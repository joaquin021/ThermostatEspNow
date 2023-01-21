#include <Arduino.h>
#include "ThermostatData.hpp"

ThermostatData thermostatData;

#include "EventsDispatcher.hpp"
#include "ShtUtils.hpp"
#include "TftUtils.hpp"
#include "ThermostatManager.hpp"
#include "ConnectivityUtils.hpp"

void setup() {
    Serial.begin(115200);
    Serial.println();
    setupConnectivity();
    pinMode(RELAY_PIN, OUTPUT);
    thermostatOff();
    refreshShtMeasures(true);
    initTft();
    addEvent(EVENT_TYPES::CONNECTIVITY);
    dispatchEvent();
}

void loop() {
    refreshShtMeasures();
    getTopic();
    detectToutch();
    checkThermostatStatus();
    //checkIfWiFiStatusHasChanged();
    //server.handleClient();
    dispatchEvent();
    refreshData();
    //MDNS.update();
}