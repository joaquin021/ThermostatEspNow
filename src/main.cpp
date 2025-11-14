#include <Arduino.h>

#include "Commons.hpp"
#include "ConnectivityUtils.hpp"
#include "EventQueue.hpp"
#include "EventsDispatcher.hpp"
#include "Secrets.h"
#include "ShtUtils.hpp"
#include "TftUtils.hpp"
#include "ThermostatManager.hpp"

// Create connectivity configuration from secrets
ConnectivityConfig connectivityConfig = {
    .wifiSSID = WIFI_SSID,
    .wifiPassword = WIFI_PASSWORD,
    .mqttBroker = MQTT_BROKER,
    .mqttPort = MQTT_PORT,
    .mqttUser = MQTT_USER,
    .mqttPassword = MQTT_PASSWORD,
    .mqttBaseTopic = MQTT_BASE_TOPIC,
    .clientName = clientName
};

ShtUtils shtUtils;
ThermostatManager thermostatManager;
TftUtils tftUtils;
ConnectivityUtils connectivityUtils(connectivityConfig);
EventsDispatcher eventsDispatcher(&thermostatManager, &tftUtils, &connectivityUtils);

void setup() {
    Serial.begin(115200);
    logDebugln("");
    thermostatManager.setup();
    EventQueue::getInstance().addEvent(EVENT_TYPES::CONNECTIVITY);
    shtUtils.refreshShtMeasures(true);
    tftUtils.initTft();
    eventsDispatcher.dispatchEvent();
}

void loop() {
    shtUtils.refreshShtMeasures();
    connectivityUtils.checkTopics();
    tftUtils.detectToutch();
    thermostatManager.checkThermostatStatus();
    eventsDispatcher.dispatchEvent();
    connectivityUtils.refreshData();
}