#include "EventsDispatcher.hpp"

EventsDispatcher::EventsDispatcher(ThermostatManager *thermostatManager, TftUtils *tftUtils, ConnectivityUtils *connectivityUtils) {
    this->thermostatManager = thermostatManager;
    this->tftUtils = tftUtils;
    this->connectivityUtils = connectivityUtils;
}

void EventsDispatcher::dispatchEvent() {
    if (!EventQueue::getInstance().isEmpty()) {
        Serial.println("EventsDispatcher.hpp\t\t########## Dispatching events ##########");
        while (!EventQueue::getInstance().isEmpty()) {
            EVENT_TYPES eventType = EventQueue::getInstance().getFrontEvent();
            Serial.printf("EventsDispatcher.hpp\t\t---> Queue size: %i\n", EventQueue::getInstance().size());
            Serial.println("EventsDispatcher.hpp\t\t---> Dispatch event: " + EventQueue::getInstance().getEventDescription(eventType));
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
            EventQueue::getInstance().removeFrontEvent();
        }
        Serial.println("EventsDispatcher.hpp\t\t########################################");
    }
}

void EventsDispatcher::manageRoomMeasuresEvent() {
    tftUtils->updateRoomTemp();
    connectivityUtils->publishTemperatureAndHumidity();
}

void EventsDispatcher::manageTargetTemperatureEvent() {
    tftUtils->updateTargetTemp();
    connectivityUtils->publishTargetTemperature();
}

void EventsDispatcher::manageModeEvent() {
    tftUtils->drawPowerButton();
    connectivityUtils->publishStatus();
}

void EventsDispatcher::manageActionEvent() {
    if (strcmp(ThermostatData::getInstance().getAction(), "heating") == 0) {
        thermostatManager->thermostatHeating();
    } else {
        thermostatManager->thermostatOff();
    }
    tftUtils->updateCircleColor();
    connectivityUtils->publishStatus();
}

void EventsDispatcher::manageConnectivityEvent() {
    tftUtils->drawWiFiButton(ILI9341_DARKCYAN);
}