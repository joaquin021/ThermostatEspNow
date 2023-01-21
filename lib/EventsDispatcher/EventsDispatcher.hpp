#ifndef __EVENTS_DISPATCHER_HPP
#define __EVENTS_DISPATCHER_HPP

#include <Arduino.h>

#include "Commons.hpp"
#include "ConnectivityUtils.hpp"
#include "EventQueue.hpp"
#include "TftUtils.hpp"
#include "ThermostatData.hpp"
#include "ThermostatManager.hpp"

class EventsDispatcher {
   private:
    ThermostatManager *thermostatManager;
    TftUtils *tftUtils;
    ConnectivityUtils *connectivityUtils;
    void manageRoomMeasuresEvent();
    void manageTargetTemperatureEvent();
    void manageModeEvent();
    void manageActionEvent();
    void manageConnectivityEvent();

   public:
    EventsDispatcher(ThermostatManager *thermostatManager, TftUtils *tftUtils, ConnectivityUtils *connectivityUtils);
    void dispatchEvent();
};

#endif
