#ifndef __THERMOSTAT_DATA_H
#define __THERMOSTAT_DATA_H

#include <WString.h>

class ThermostatData {
   private:
    float temperature = 0;
    float humidity = 0;
    float hotTolerance = 0.5;
    float coldTolerance = 0.5;
    float temperatureStep = 0.5;
    float targetTemp = 21;
    String action = "off";
    char mode[5] = "off";
    bool connectivityActive = false;

   public:
    float getTemperature();
    void setTemperature(float newTemperature);
    float getHumidity();
    void setHumidity(float newHumidity);
    float getHotTolerance();
    void setHotTolerance(String newHotTolerance);
    float getColdTolerance();
    void setColdTolerance(String newColdTolerance);
    float getTargetTemp();
    void setTargetTemp(float newTargetTemp);
    void increaseTargetTemp();
    void decreaseTargetTemp();
    void setTemperatureStep(String newTemperatureStep);
    String getAction();
    void setAction(String newAction);
    char *getMode();
    void changeMode(const char *newMode, unsigned int length);
    void changeMode(uint8_t *newMode, unsigned int length);
    bool isConnectivityActive();
    bool toggleConnectivity();
};

#endif