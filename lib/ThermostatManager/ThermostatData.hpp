#ifndef __THERMOSTAT_DATA_H
#define __THERMOSTAT_DATA_H

#include <Arduino.h>

class ThermostatData {
   private:
    float temperature = 0;
    float humidity = 0;
    float hotTolerance = 0.5;
    float coldTolerance = 0.5;
    float temperatureStep = 0.5;
    float targetTemp = 21;
    char action[8] = "off";
    char mode[5] = "off";
    bool connectivityActive = true;
    ThermostatData() {}

   public:
    static ThermostatData &getInstance() {
        static ThermostatData instance;
        return instance;
    }
    float getTemperature();
    void setTemperature(float newTemperature);
    float getHumidity();
    void setHumidity(float newHumidity);
    float getHotTolerance();
    float getColdTolerance();
    float getTargetTemp();
    void setTargetTemp(float newTargetTemp);
    void increaseTargetTemp();
    void decreaseTargetTemp();
    char *getAction();
    void setAction(const char *newAction);
    char *getMode();
    void changeMode(const char *newMode);
    bool isConnectivityActive();
    bool toggleConnectivity();
};

#endif