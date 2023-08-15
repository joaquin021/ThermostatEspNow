#include "ThermostatData.hpp"

float ThermostatData::getTemperature() { return temperature; }

void ThermostatData::setTemperature(float newTemperature) { temperature = newTemperature; }

float ThermostatData::getHumidity() { return humidity; }

void ThermostatData::setHumidity(float newHumidity) { humidity = newHumidity; }

float ThermostatData::getHotTolerance() { return hotTolerance; }

float ThermostatData::getColdTolerance() { return coldTolerance; }

float ThermostatData::getTargetTemp() { return targetTemp; }

void ThermostatData::setTargetTemp(float newTargetTemp) {
    targetTemp = newTargetTemp;
}

void ThermostatData::increaseTargetTemp() { targetTemp = targetTemp + temperatureStep; }

void ThermostatData::decreaseTargetTemp() { targetTemp = targetTemp - temperatureStep; }

char *ThermostatData::getAction() { return action; }

void ThermostatData::setAction(const char *newAction) {
    strcpy(action, newAction);
}

char *ThermostatData::getMode() { return mode; }

void ThermostatData::changeMode(const char *newMode) {
    strcpy(mode, newMode);
}

bool ThermostatData::isConnectivityActive() { return connectivityActive; }

bool ThermostatData::toggleConnectivity() {
    connectivityActive = !connectivityActive;
    return connectivityActive;
}