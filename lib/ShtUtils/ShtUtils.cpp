#include "ShtUtils.hpp"

unsigned long NEXT_LOAD_SENSOR_TIME = 0;
unsigned long NEXT_LOAD_SENSOR_PERIOD = 60000;

ShtUtils::ShtUtils(): sht30(0x45){}

void ShtUtils::refreshShtMeasures(bool resetSensorTime) {
    if (NEXT_LOAD_SENSOR_TIME < millis()) {
        sht30.get();
        ThermostatData::getInstance().setTemperature(sht30.cTemp);
        ThermostatData::getInstance().setHumidity(sht30.humidity);
        EventQueue::getInstance().addEvent(EVENT_TYPES::ROOM_MEASURES);
        NEXT_LOAD_SENSOR_TIME = resetSensorTime ? 0 : millis() + NEXT_LOAD_SENSOR_PERIOD;
    }
}