#ifndef __CONNECTIVITY_UTILS_HPP
#define __CONNECTIVITY_UTILS_HPP

#include "EspNowService.hpp"
#include "EventQueue.hpp"
#include "RequestUtils.hpp"
#include "ThermostatData.hpp"
#include "WifiUtils.hpp"

class ConnectivityUtils {
   private:
    EspNowService espNowService;
    const char *clientName;
    uint8_t *gatewayAddress;
    uint8_t *clientAdress;
    void buildTemperatureRequest(request *request);
    void buildHumidityRequest(request *request);
    void buildTargetTemperatureRequest(request *request);
    void buildAvailability(request *request);
    void buildMode(request *request);
    void buildAction(request *request);
    void sendRequest(request *request);

   public:
    ConnectivityUtils(const char *clientName, uint8_t *gatewayAddress, uint8_t *clientAdress);
    void setupConnectivity();
    void publishTemperatureAndHumidity();
    void publishTargetTemperature();
    void publishStatus();
    void getTopic();
    void refreshData(bool force = false);
};

#endif