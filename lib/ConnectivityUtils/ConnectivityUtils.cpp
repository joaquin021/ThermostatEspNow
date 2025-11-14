#include "ConnectivityUtils.hpp"
#include <WiFi.h>
#include <PubSubClient.h>

unsigned long NEXT_REFRESH_TIME_FOR_REFRESH_DATA = millis();
unsigned long NEXT_REFRESH_PERIOD_FOR_REFRESH_DATA = 300000;

unsigned long NEXT_REFRESH_TIME_FOR_CHECK_TOPICS = millis();
unsigned long NEXT_REFRESH_PERIOD_FOR_CHECK_TOPICS = 100;  // Check every 100 ms for MQTT loop

char lastStatusMessage[6] = "none";

// Static instance pointer for callback
static ConnectivityUtils* instancePtr = nullptr;

void manageSendResult(bool success) {
    if (success && strcmp(lastStatusMessage, "true") != 0) {
        strcpy(lastStatusMessage, "true");
        EventQueue::getInstance().addEvent(EVENT_TYPES::MESSAGE_OK);
    } else if (!success && strcmp(lastStatusMessage, "false") != 0) {
        strcpy(lastStatusMessage, "false");
        EventQueue::getInstance().addEvent(EVENT_TYPES::MESSAGE_FAILED);
    }
}

ConnectivityUtils::ConnectivityUtils(const ConnectivityConfig& config) : config(config) {
    this->lastReconnectAttempt = 0;
    this->wifiClient = new WiFiClient();
    this->mqttClient = new PubSubClient(*wifiClient);
    instancePtr = this;
}

void ConnectivityUtils::connectWiFi() {
    logInfoln("Connecting to WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.wifiSSID, config.wifiPassword);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        logInfo(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        logInfoln("");
        logInfo("WiFi connected! IP: ");
        logInfoln(WiFi.localIP());
    } else {
        logErrorln("WiFi connection failed!");
    }
}

bool ConnectivityUtils::connectMQTT() {
    if (mqttClient->connected()) {
        return true;
    }

    logInfoln("Connecting to MQTT broker...");

    // Set MQTT server and callback
    mqttClient->setServer(config.mqttBroker, config.mqttPort);
    mqttClient->setCallback(mqttCallback);

    // Attempt to connect
    bool connected = false;
    if (strlen(config.mqttUser) > 0) {
        connected = mqttClient->connect(config.clientName, config.mqttUser, config.mqttPassword);
    } else {
        connected = mqttClient->connect(config.clientName);
    }

    if (connected) {
        logInfoln("MQTT connected!");
        subscribeMQTT();
        return true;
    } else {
        logError("MQTT connection failed, rc=");
        logErrorln(mqttClient->state());
        return false;
    }
}

void ConnectivityUtils::subscribeMQTT() {
    char topic[100];

    // Subscribe to mode changes
    snprintf(topic, sizeof(topic), "%s/chg/mode", instancePtr->config.mqttBaseTopic);
    mqttClient->subscribe(topic);
    logInfo("Subscribed to: ");
    logInfoln(topic);

    // Subscribe to target temperature changes
    snprintf(topic, sizeof(topic), "%s/chg/tgTemp", instancePtr->config.mqttBaseTopic);
    mqttClient->subscribe(topic);
    logInfo("Subscribed to: ");
    logInfoln(topic);
}

void ConnectivityUtils::mqttCallback(char* topic, byte* payload, unsigned int length) {
    // Convert payload to string
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';

    logInfo("MQTT message received on topic: ");
    logInfoln(topic);
    logInfo("Message: ");
    logInfoln(message);

    // Parse topic to determine action
    char expectedModeTopic[100];
    char expectedTempTopic[100];
    snprintf(expectedModeTopic, sizeof(expectedModeTopic), "%s/chg/mode", instancePtr->config.mqttBaseTopic);
    snprintf(expectedTempTopic, sizeof(expectedTempTopic), "%s/chg/tgTemp", instancePtr->config.mqttBaseTopic);

    if (strcmp(topic, expectedModeTopic) == 0) {
        // Mode change
        ThermostatData::getInstance().changeMode(message);
        EventQueue::getInstance().addEvent(EVENT_TYPES::MODE);
        logInfo("Mode changed to: ");
        logInfoln(message);
    } else if (strcmp(topic, expectedTempTopic) == 0) {
        // Target temperature change
        float newTemp = atof(message);
        ThermostatData::getInstance().setTargetTemp(newTemp);
        EventQueue::getInstance().addEvent(EVENT_TYPES::TARGET_TEMPERATURE);
        logInfo("Target temperature changed to: ");
        logInfoln(newTemp);
    }
}

void ConnectivityUtils::publishMQTT(const char* topic, const char* payload) {
    char fullTopic[100];
    snprintf(fullTopic, sizeof(fullTopic), "%s/%s", config.mqttBaseTopic, topic);

    bool success = mqttClient->publish(fullTopic, payload, false);  // QoS 0, no retain

    if (success) {
        logTrace("Published to ");
        logTrace(fullTopic);
        logTrace(": ");
        logTraceln(payload);
    } else {
        logError("Failed to publish to ");
        logErrorln(fullTopic);
    }

    manageSendResult(success);
}

void ConnectivityUtils::setupConnectivity() {
    if (ThermostatData::getInstance().isConnectivityActive()) {
        connectWiFi();
        if (WiFi.status() == WL_CONNECTED) {
            connectMQTT();
            refreshData(true);
        }
    }
    strcpy(lastStatusMessage, "none");
}

void ConnectivityUtils::disconnect() {
    publishStatus();
    delay(500);
    mqttClient->disconnect();
    WiFi.mode(WIFI_OFF);
    strcpy(lastStatusMessage, "none");
}

bool ConnectivityUtils::isConnected() {
    return WiFi.status() == WL_CONNECTED && mqttClient->connected();
}

void ConnectivityUtils::publishTemperatureAndHumidity() {
    if (!isConnected()) return;

    char temperature[10];
    dtostrf(ThermostatData::getInstance().getTemperature(), 5, 2, temperature);
    publishMQTT("temperature", temperature);

    char humidity[10];
    dtostrf(ThermostatData::getInstance().getHumidity(), 5, 2, humidity);
    publishMQTT("humidity", humidity);
}

void ConnectivityUtils::publishTargetTemperature() {
    if (!isConnected()) return;

    char temperature[10];
    dtostrf(ThermostatData::getInstance().getTargetTemp(), 5, 2, temperature);
    publishMQTT("targetTemp", temperature);
}

void ConnectivityUtils::publishStatus() {
    if (!isConnected()) return;

    // Publish availability
    const char* availability = ThermostatData::getInstance().isConnectivityActive() ? "online" : "offline";
    publishMQTT("available", availability);

    // Publish mode
    publishMQTT("mode", ThermostatData::getInstance().getMode());

    // Publish action
    publishMQTT("action", ThermostatData::getInstance().getAction());
}

void ConnectivityUtils::checkTopics() {
    if (NEXT_REFRESH_TIME_FOR_CHECK_TOPICS < millis()) {
        // Maintain MQTT connection and process incoming messages
        if (ThermostatData::getInstance().isConnectivityActive()) {
            if (!mqttClient->connected()) {
                unsigned long now = millis();
                if (now - lastReconnectAttempt > 5000) {  // Try to reconnect every 5 seconds
                    lastReconnectAttempt = now;
                    logInfoln("Attempting MQTT reconnection...");
                    if (WiFi.status() != WL_CONNECTED) {
                        connectWiFi();
                    }
                    if (WiFi.status() == WL_CONNECTED) {
                        connectMQTT();
                    }
                }
            } else {
                mqttClient->loop();  // Process MQTT messages
            }
        }
        NEXT_REFRESH_TIME_FOR_CHECK_TOPICS = millis() + NEXT_REFRESH_PERIOD_FOR_CHECK_TOPICS;
    }
}

void ConnectivityUtils::refreshData(bool force) {
    if (isConnected() && (force || NEXT_REFRESH_TIME_FOR_REFRESH_DATA < millis())) {
        publishStatus();
        publishTargetTemperature();
        publishTemperatureAndHumidity();
        NEXT_REFRESH_TIME_FOR_REFRESH_DATA = millis() + NEXT_REFRESH_PERIOD_FOR_REFRESH_DATA;
    }
}
