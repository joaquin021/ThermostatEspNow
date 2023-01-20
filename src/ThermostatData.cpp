#include "ThermostatData.hpp"

#include <Arduino.h>

#include <cstring>

float ThermostatData::getTemperature() { return temperature; }

void ThermostatData::setTemperature(float newTemperature) { temperature = newTemperature; }

float ThermostatData::getHumidity() { return humidity; }

void ThermostatData::setHumidity(float newHumidity) { humidity = newHumidity; }

float ThermostatData::getHotTolerance() { return hotTolerance; }

void ThermostatData::setHotTolerance(String newHotTolerance) {
    if (!newHotTolerance.isEmpty()) {
        Serial.println("ThermostatData.cpp\t\tSettig new hot tolerance.");
        hotTolerance = atof(newHotTolerance.c_str());
    }
}

float ThermostatData::getColdTolerance() { return coldTolerance; }

void ThermostatData::setColdTolerance(String newColdTolerance) {
    if (!newColdTolerance.isEmpty()) {
        Serial.println("ThermostatData.cpp\t\tSettig new cold tolerance.");
        coldTolerance = atof(newColdTolerance.c_str());
    }
}

float ThermostatData::getTargetTemp() { return targetTemp; }

void ThermostatData::setTargetTemp(float newTargetTemp) {
    targetTemp = newTargetTemp;
}

void ThermostatData::increaseTargetTemp() { targetTemp = targetTemp + temperatureStep; }

void ThermostatData::decreaseTargetTemp() { targetTemp = targetTemp - temperatureStep; }

void ThermostatData::setTemperatureStep(String newTemperatureStep) {
    if (!newTemperatureStep.isEmpty()) {
        Serial.println("ThermostatData.cpp\t\tSettig new temperature step.");
        temperatureStep = atof(newTemperatureStep.c_str());
    }
}

String ThermostatData::getAction() { return action; }

void ThermostatData::setAction(String newAction) { action = newAction; }

char *ThermostatData::getMode() { return mode; }

void ThermostatData::changeMode(const char *newMode, unsigned int length) {
    strncpy(mode, newMode, length - 1);
    mode[length - 1] = '\0';
}

void ThermostatData::changeMode(uint8_t *newMode, unsigned int length) {
    memcpy(mode, newMode, length);
    mode[length] = '\0';
}

bool ThermostatData::isConnectivityActive() { return connectivityActive; }

bool ThermostatData::toggleConnectivity() {
    connectivityActive = !connectivityActive;
    return connectivityActive;
}