#ifndef __CONNECTIVITY_UTILS_HPP
#define __CONNECTIVITY_UTILS_HPP

#include "EspNowService.hpp"
#include "RequestUtils.hpp"
#include "Secrets.h"
#include "WifiUtils.hpp"

unsigned long NEXT_REFRESH_TIME = millis();
unsigned long NEXT_REFRESH_PERIOD = 300000;

unsigned long NEXT_REFRESH_TIME2 = millis();
unsigned long NEXT_REFRESH_PERIOD2 = 1000;

EspNowService espNowService;
RequestUtils requestUtils = RequestUtils::getInstance();
ResponseUtils responseUtils = ResponseUtils::getInstance();

void opResponseHandler(response *response, response_OpResponse *opResponse, int operationNumber) {
    if (response->message_type == 631 && opResponse->result_code == 0) {
        debugln("+++++++++++++++++++++++++++++++++++++++++++++++++++++");
        if (operationNumber == 0) {
            debugln(opResponse->payload);
            thermostatData.changeMode(opResponse->payload, strlen(opResponse->payload));
            addEvent(EVENT_TYPES::MODE);
        } else {
            char auxTargetTemp[strlen(opResponse->payload)];
            memcpy(&auxTargetTemp, opResponse->payload, strlen(opResponse->payload));
            thermostatData.setTargetTemp(String(auxTargetTemp).toFloat());
            addEvent(EVENT_TYPES::TARGET_TEMPERATURE);
        }
        debugln("-------------------------------------------------------");
    }
}

void espNowRecvCallBack(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    responseUtils.manage(data, data_len, responseHandlerDummy, opResponseHandler);
}

void setupConnectivity() {
    setupWiFiForEspNow();
    espNowService.setup(espNowSendCallBackDummy, espNowRecvCallBack);
}

void sendRequest(request *request) {
    uint8_t serializedBuffer[ESPNOW_BUFFERSIZE];
    int serializedLen = requestUtils.serialize(serializedBuffer, request);
    espNowService.send(gatewayAddress, serializedBuffer, serializedLen);
}

void buildTemperatureRequest(request *request) {
    char temperature[6];
    dtostrf(thermostatData.getTemperature(), 5, 2, temperature);
    requestUtils.buildSendOperation(request, temperature, "temperature");
}

void buildHumidityRequest(request *request) {
    char humidity[6];
    dtostrf(thermostatData.getHumidity(), 5, 2, humidity);
    requestUtils.buildSendOperation(request, humidity, "humidity");
}

void buildTargetTemperatureRequest(request *request) {
    char temperature[6];
    dtostrf(thermostatData.getTargetTemp(), 5, 2, temperature);
    requestUtils.buildSendOperation(request, temperature, "targetTemp");
}

void buildAvailability(request *request) {
    requestUtils.buildSendOperation(request, "online", "available");
}

void buildMode(request *request) {
    requestUtils.buildSendOperation(request, thermostatData.getMode(), "mode");
}

void buildAction(request *request) {
    requestUtils.buildSendOperation(request, thermostatData.getAction().c_str(), "action");
}

void publishTemperatureAndHumidity() {
    request request = requestUtils.createRequest(clientName, clientAdress, 3);
    buildTemperatureRequest(&request);
    buildHumidityRequest(&request);
    sendRequest(&request);
}

void publishTargetTemperature() {
    request request = requestUtils.createRequest(clientName, clientAdress, 2);
    buildTargetTemperatureRequest(&request);
    sendRequest(&request);
}

void publishStatus() {
    request request = requestUtils.createRequest(clientName, clientAdress, 1);
    buildAvailability(&request);
    buildMode(&request);
    buildAction(&request);
    sendRequest(&request);
}

void getTopic() {
    if (NEXT_REFRESH_TIME2 < millis()) {
        request request = requestUtils.createRequest(clientName, clientAdress, 631);
        requestUtils.buildSubscribeOperation(&request, "chg/mode");
        requestUtils.buildSubscribeOperation(&request, "chg/tgTemp");
        sendRequest(&request);
        NEXT_REFRESH_TIME2 = millis() + NEXT_REFRESH_PERIOD2;
    }
}

void refreshData(bool force = false) {
    if (force || NEXT_REFRESH_TIME < millis()) {
        publishStatus();
        publishTargetTemperature();
        publishTemperatureAndHumidity();
        NEXT_REFRESH_TIME = millis() + NEXT_REFRESH_PERIOD;
    }
}

#endif