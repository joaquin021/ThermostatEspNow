#ifndef __THERMOSTAT_MANAGER_HPP
#define __THERMOSTAT_MANAGER_HPP

#include "Arduino.h"
#include "Commons.hpp"
#include "EventQueue.hpp"
#include "ThermostatData.hpp"

class ThermostatManager {
   private:
    boolean temperatureAboveTarget();
    boolean temperatureBelowTarget();

   public:
    ThermostatManager();
    void setup();
    void checkThermostatStatus();
    void thermostatHeating();
    void thermostatOff();
};

#endif