#ifndef __SHT_UTILS_HPP
#define __SHT_UTILS_HPP

#include <WEMOS_SHT3X.h>

#include "EventQueue.hpp"
#include "ThermostatData.hpp"

class ShtUtils {
   private:
    SHT3X sht30;

   public:
    ShtUtils();
    void refreshShtMeasures(bool resetSensorTime = false);
};

#endif