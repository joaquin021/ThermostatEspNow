#ifndef __EVENT_UTILS_HPP
#define __EVENT_UTILS_HPP

#include <Arduino.h>

#include <queue>

#include "Commons.hpp"

enum EVENT_TYPES { ROOM_MEASURES,
                   TARGET_TEMPERATURE,
                   MODE,
                   ACTION,
                   CONNECTIVITY,
                   MESSAGE_OK,
                   MESSAGE_FAILED };

class EventQueue {
   public:
    static EventQueue& getInstance() {
        static EventQueue instance;
        return instance;
    }
    void addEvent(EVENT_TYPES eventType);
    EVENT_TYPES getFrontEvent();
    void removeFrontEvent();
    int size();
    bool isEmpty();

    String getEventDescription(EVENT_TYPES eventType);

   private:
    EventQueue() {}
    std::queue<EVENT_TYPES> events;
};

#endif