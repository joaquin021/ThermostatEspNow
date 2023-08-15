#include <Arduino.h>

#include "Commons.hpp"
#include "ConnectivityUtils.hpp"
#include "EventQueue.hpp"
#include "EventsDispatcher.hpp"
#include "Secrets.h"
#include "ShtUtils.hpp"
#include "TftUtils.hpp"
#include "ThermostatManager.hpp"

ShtUtils shtUtils;
ThermostatManager thermostatManager;
TftUtils tftUtils;
ConnectivityUtils connectivityUtils(clientName, gatewayAddress, clientAdress);
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