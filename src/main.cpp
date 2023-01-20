#include <Arduino.h>
#include "ThermostatData.hpp"
ThermostatData thermostatData;

#include "EventsDispatcher.hpp"
#include "ShtUtils.hpp"
#include "TftUtils.hpp"
#include "ThermostatManager.hpp"

void setup() {
    Serial.begin(115200);
    Serial.println();
    pinMode(RELAY_PIN, OUTPUT);
    thermostatOff();
    refreshShtMeasures(true);
    initTft();
    addEvent(EVENT_TYPES::CONNECTIVITY);
    dispatchEvent();
    //initServer();
}

void loop() {
    refreshShtMeasures();
    //mqttClient.loop();
    detectToutch();
    checkThermostatStatus();
    //checkIfWiFiStatusHasChanged();
    //server.handleClient();
    dispatchEvent();
    //refreshMqttData();
    //MDNS.update();
}