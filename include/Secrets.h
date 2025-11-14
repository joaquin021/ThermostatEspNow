#ifndef __SECRETS_H
#define __SECRETS_H

#include <Arduino.h>

// Client name for MQTT topics
const char* clientName = "thermo";

// WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// MQTT broker configuration
const char* MQTT_BROKER = "192.168.1.100";  // Replace with your MQTT broker IP
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";                  // Empty if no authentication
const char* MQTT_PASSWORD = "";              // Empty if no authentication

// MQTT base topic
const char* MQTT_BASE_TOPIC = "ESPNowToMqttGateway/thermo";

#endif