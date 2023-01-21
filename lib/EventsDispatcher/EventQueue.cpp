#include "EventQueue.hpp"

String EVENT_TYPES_DESCRIPTION[] = {"ROOM_MEASURES", "TARGET_TEMPERATURE", "MODE", "ACTION", "CONECTIVITY"};

void EventQueue::addEvent(EVENT_TYPES eventType) {
    events.push(eventType);
    logDebugln("EventUtils.hpp\t\t\tAdded event: " + EVENT_TYPES_DESCRIPTION[eventType]);
}

EVENT_TYPES EventQueue::getFrontEvent() {
    return events.front();
}

void EventQueue::removeFrontEvent() {
    events.pop();
}

int EventQueue::size() {
    return events.size();
}

bool EventQueue::isEmpty() {
    return events.empty();
}

String EventQueue::getEventDescription(EVENT_TYPES eventType) {
    return EVENT_TYPES_DESCRIPTION[eventType];
}