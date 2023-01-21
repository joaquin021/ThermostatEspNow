#include <Arduino.h>

#include "ConnectivityUtils.hpp"
#include "EventsDispatcher.hpp"
#include "ShtUtils.hpp"
#include "TftUtils.hpp"
#include "ThermostatData.hpp"
#include "ThermostatManager.hpp"
#include "Secrets.h"

ShtUtils shtUtils;
ThermostatManager thermostatManager;
TftUtils tftUtils;
ConnectivityUtils connectivityUtils(clientName, gatewayAddress, clientAdress);
EventsDispatcher eventsDispatcher(&thermostatManager, &tftUtils, &connectivityUtils);

void setup() {
    Serial.begin(115200);
    Serial.println();
    thermostatManager.setup();
    connectivityUtils.setupConnectivity();
    shtUtils.refreshShtMeasures(true);
    tftUtils.initTft();
    // addEvent(EVENT_TYPES::CONNECTIVITY);
    eventsDispatcher.dispatchEvent();
}

void loop() {
    shtUtils.refreshShtMeasures();
    connectivityUtils.getTopic();
    tftUtils.detectToutch();
    thermostatManager.checkThermostatStatus();
    eventsDispatcher.dispatchEvent();
    connectivityUtils.refreshData();
}