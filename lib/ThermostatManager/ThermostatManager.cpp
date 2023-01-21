#include "ThermostatManager.hpp"

#define RELAY_PIN 10

ThermostatManager::ThermostatManager() {}

void ThermostatManager::setup() {
    pinMode(RELAY_PIN, OUTPUT);
    thermostatOff();
}

void ThermostatManager::checkThermostatStatus() {
    if (strcmp(ThermostatData::getInstance().getMode(), "heat") == 0) {
        if (strcmp(ThermostatData::getInstance().getAction(), "idle") != 0 && temperatureAboveTarget()) {
            ThermostatData::getInstance().setAction("idle");
            EventQueue::getInstance().addEvent(EVENT_TYPES::ACTION);
        } else if (strcmp(ThermostatData::getInstance().getAction(), "heating") != 0 && temperatureBelowTarget()) {
            ThermostatData::getInstance().setAction("heating");
            EventQueue::getInstance().addEvent(EVENT_TYPES::ACTION);
        } else if (strcmp(ThermostatData::getInstance().getAction(), "off") == 0) {
            ThermostatData::getInstance().setAction("idle");
            EventQueue::getInstance().addEvent(EVENT_TYPES::ACTION);
        }
    } else if (strcmp(ThermostatData::getInstance().getAction(), "off") != 0) {
        ThermostatData::getInstance().setAction("off");
        EventQueue::getInstance().addEvent(EVENT_TYPES::ACTION);
    }
}

void ThermostatManager::thermostatHeating() {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("ThermostatManager.hpp\t\tRelay HIGH. Thermostat heating.");
}

void ThermostatManager::thermostatOff() {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("ThermostatManager.hpp\t\tRelay LOW. Thermostat off.");
}

boolean ThermostatManager::temperatureAboveTarget() {
    return ThermostatData::getInstance().getTemperature() > ThermostatData::getInstance().getTargetTemp() + ThermostatData::getInstance().getHotTolerance();
}

boolean ThermostatManager::temperatureBelowTarget() {
    return ThermostatData::getInstance().getTemperature() < ThermostatData::getInstance().getTargetTemp() - ThermostatData::getInstance().getColdTolerance();
}