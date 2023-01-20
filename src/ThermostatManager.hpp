#ifndef __THERMOSTAT_MANAGER_H
#define __THERMOSTAT_MANAGER_H

#include "EventUtils.hpp"

#define RELAY_PIN 10

boolean temperatureAboveTarget() {
    return thermostatData.getTemperature() > thermostatData.getTargetTemp() + thermostatData.getHotTolerance();
}

boolean temperatureBelowTarget() {
    return thermostatData.getTemperature() < thermostatData.getTargetTemp() - thermostatData.getColdTolerance();
}

void checkThermostatStatus() {
    if (strcmp(thermostatData.getMode(), "heat") == 0) {
        if (!thermostatData.getAction().equals("idle") && temperatureAboveTarget()) {
            thermostatData.setAction("idle");
            addEvent(EVENT_TYPES::ACTION);
        } else if (!thermostatData.getAction().equals("heating") && temperatureBelowTarget()) {
            thermostatData.setAction("heating");
            addEvent(EVENT_TYPES::ACTION);
        } else if (thermostatData.getAction().equals("off")) {
            thermostatData.setAction("idle");
            addEvent(EVENT_TYPES::ACTION);
        }
    } else if (!thermostatData.getAction().equals("off")) {
        thermostatData.setAction("off");
        addEvent(EVENT_TYPES::ACTION);
    }
}

void thermostatHeating() {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("ThermostatManager.hpp\t\tRelay HIGH. Thermostat heating.");
}

void thermostatOff() {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("ThermostatManager.hpp\t\tRelay LOW. Thermostat off.");
}

#endif