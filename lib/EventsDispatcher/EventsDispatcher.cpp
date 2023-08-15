#include "EventsDispatcher.hpp"

EventsDispatcher::EventsDispatcher(ThermostatManager *thermostatManager, TftUtils *tftUtils, ConnectivityUtils *connectivityUtils) {
    this->thermostatManager = thermostatManager;
    this->tftUtils = tftUtils;
    this->connectivityUtils = connectivityUtils;
}

void EventsDispatcher::dispatchEvent() {
    if (!EventQueue::getInstance().isEmpty()) {
        logDebugln("EventsDispatcher.hpp\t\t########## Dispatching events ##########");
        while (!EventQueue::getInstance().isEmpty()) {
            EVENT_TYPES eventType = EventQueue::getInstance().getFrontEvent();
            logDebugf("EventsDispatcher.hpp\t\t---> Queue size: %i\n", EventQueue::getInstance().size());
            logDebugln("EventsDispatcher.hpp\t\t---> Dispatch event: " + EventQueue::getInstance().getEventDescription(eventType));
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
                case EVENT_TYPES::MESSAGE_OK:
                    manageMessageOk();
                    break;
                case EVENT_TYPES::MESSAGE_FAILED:
                    manageMessageFailed();
                    break;
                default:
                    break;
            }
            EventQueue::getInstance().removeFrontEvent();
        }
        logDebugln("EventsDispatcher.hpp\t\t########################################");
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
    if (ThermostatData::getInstance().isConnectivityActive()) {
        tftUtils->drawWiFiButton(ILI9341_DARKCYAN);
        connectivityUtils->setupConnectivity();
    } else {
        tftUtils->drawWiFiButton(ILI9341_ULTRA_DARKGREY);
        connectivityUtils->disconnect();
    }
}

void EventsDispatcher::manageMessageOk() {
    tftUtils->drawWiFiButton(ILI9341_WHITE);
}

void EventsDispatcher::manageMessageFailed() {
    tftUtils->drawWiFiButton(ILI9341_RED);
}