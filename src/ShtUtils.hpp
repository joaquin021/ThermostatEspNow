#ifndef __SHT_UTILS_H
#define __SHT_UTILS_H

#include <WEMOS_SHT3X.h>

#include "EventUtils.hpp"

SHT3X sht30(0x45);

unsigned long NEXT_LOAD_SENSOR_TIME = 0;
unsigned long NEXT_LOAD_SENSOR_PERIOD = 60000;

void refreshShtMeasures(bool resetSensorTime = false) {
    if (NEXT_LOAD_SENSOR_TIME < millis()) {
        sht30.get();
        thermostatData.setTemperature(sht30.cTemp);
        thermostatData.setHumidity(sht30.humidity);
        addEvent(EVENT_TYPES::ROOM_MEASURES);
        NEXT_LOAD_SENSOR_TIME = resetSensorTime ? 0 : millis() + NEXT_LOAD_SENSOR_PERIOD;
    }
}

#endif