#ifndef __CONNECTIVITY_UTILS_HPP
#define __CONNECTIVITY_UTILS_HPP

#include "Commons.hpp"
#include "EventQueue.hpp"
#include "ThermostatData.hpp"

// Forward declarations
class WiFiClient;
class PubSubClient;

// Configuration structure
struct ConnectivityConfig {
    const char* wifiSSID;
    const char* wifiPassword;
    const char* mqttBroker;
    int mqttPort;
    const char* mqttUser;
    const char* mqttPassword;
    const char* mqttBaseTopic;
    const char* clientName;
};

class ConnectivityUtils {
   private:
    WiFiClient* wifiClient;
    PubSubClient* mqttClient;
    ConnectivityConfig config;
    unsigned long lastReconnectAttempt;

    // Private methods
    void connectWiFi();
    bool connectMQTT();
    void publishMQTT(const char* topic, const char* payload);
    void subscribeMQTT();
    static void mqttCallback(char* topic, byte* payload, unsigned int length);

   public:
    ConnectivityUtils(const ConnectivityConfig& config);
    void setupConnectivity();
    void disconnect();
    void publishTemperatureAndHumidity();
    void publishTargetTemperature();
    void publishStatus();
    void checkTopics();
    void refreshData(bool force = false);
    bool isConnected();
};

#endif