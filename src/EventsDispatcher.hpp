#ifndef __EVENTS_DISPATCHER_H
#define __EVENTS_DISPATCHER_H

#include <queue>

#include "EventUtils.hpp"
#include "TftUtils.hpp"
#include "ThermostatManager.hpp"
#include "ConnectivityUtils.hpp"

void manageRoomMeasuresEvent() {
    updateRoomTemp();
    publishTemperatureAndHumidity();
}

void manageTargetTemperatureEvent() {
    updateTargetTemp();
    publishTargetTemperature();
}

void manageModeEvent() {
    drawPowerButton();
    publishStatus();
}

void manageActionEvent() {
    if (thermostatData.getAction().equals("heating")) {
        thermostatHeating();
    } else {
        thermostatOff();
    }
    updateCircleColor();
    publishStatus();
}

void manageConnectivityEvent() {
    drawWiFiButton(ILI9341_DARKCYAN);
    if (thermostatData.isConnectivityActive()) {
        // connectWiFi();
    } else {
        // disconnectMqtt();
        // disconnectWiFi();
    }
}

void dispatchEvent() {
    if (!EVENTS.empty()) {
        Serial.println("EventsDispatcher.hpp\t\t########## Dispatching events ##########");
        while (!EVENTS.empty()) {
            EVENT_TYPES eventType = EVENTS.front();
            Serial.printf("EventsDispatcher.hpp\t\t---> Queue size: %i\n", EVENTS.size());
            Serial.println("EventsDispatcher.hpp\t\t---> Dispatch event: " + EVENT_TYPES_DESCRIPTION[eventType]);
            switch (eventType) {
                case EVENT_TYPES::ROOM_MEASURES:
                    manageRoomMeasuresEvent();
                    break;
                case EVENT_TYPES::TARGET_TEMPERATURE:
                    manageTargetTemperatureEvent();
                    break;
                case EVENT_TYPES::MODE:
                    manageModeEvent();
                    break;
                case EVENT_TYPES::ACTION:
                    manageActionEvent();
                    break;
                case EVENT_TYPES::CONNECTIVITY:
                    manageConnectivityEvent();
                    break;
                default:
                    break;
            }
            EVENTS.pop();
        }
        Serial.println("EventsDispatcher.hpp\t\t########################################");
    }
}

#endif
