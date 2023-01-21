#include "ConnectivityUtils.hpp"

unsigned long NEXT_REFRESH_TIME = millis();
unsigned long NEXT_REFRESH_PERIOD = 300000;

unsigned long NEXT_REFRESH_TIME2 = millis();
unsigned long NEXT_REFRESH_PERIOD2 = 1000;

RequestUtils requestUtils = RequestUtils::getInstance();
ResponseUtils responseUtils = ResponseUtils::getInstance();

void opResponseHandler(response *response, response_OpResponse *opResponse, int operationNumber) {
    if (response->message_type == 631 && opResponse->result_code == 0) {
        debugln("+++++++++++++++++++++++++++++++++++++++++++++++++++++");
        if (operationNumber == 0) {
            debugln(opResponse->payload);
            ThermostatData::getInstance().changeMode(opResponse->payload);
            EventQueue::getInstance().addEvent(EVENT_TYPES::MODE);
        } else {
            char auxTargetTemp[strlen(opResponse->payload)];
            memcpy(&auxTargetTemp, opResponse->payload, strlen(opResponse->payload));
            ThermostatData::getInstance().setTargetTemp(String(auxTargetTemp).toFloat());
            EventQueue::getInstance().addEvent(EVENT_TYPES::TARGET_TEMPERATURE);
        }
        debugln("-------------------------------------------------------");
    }
}

void espNowRecvCallBack(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    responseUtils.manage(data, data_len, responseHandlerDummy, opResponseHandler);
}

ConnectivityUtils::ConnectivityUtils(const char *clientName, uint8_t *gatewayAddress, uint8_t *clientAdress) {
    this->clientName = clientName;
    this->gatewayAddress = gatewayAddress;
    this->clientAdress = clientAdress;
}

void ConnectivityUtils::setupConnectivity() {
    setupWiFiForEspNow();
    espNowService.setup(espNowSendCallBackDummy, espNowRecvCallBack);
}

void ConnectivityUtils::publishTemperatureAndHumidity() {
    request request = requestUtils.createRequest(clientName, clientAdress, 3);
    buildTemperatureRequest(&request);
    buildHumidityRequest(&request);
    sendRequest(&request);
}

void ConnectivityUtils::publishTargetTemperature() {
    request request = requestUtils.createRequest(clientName, clientAdress, 2);
    buildTargetTemperatureRequest(&request);
    sendRequest(&request);
}

void ConnectivityUtils::publishStatus() {
    request request = requestUtils.createRequest(clientName, clientAdress, 1);
    buildAvailability(&request);
    buildMode(&request);
    buildAction(&request);
    sendRequest(&request);
}

void ConnectivityUtils::getTopic() {
    if (NEXT_REFRESH_TIME2 < millis()) {
        request request = requestUtils.createRequest(clientName, clientAdress, 631);
        requestUtils.buildSubscribeOperation(&request, "chg/mode");
        requestUtils.buildSubscribeOperation(&request, "chg/tgTemp");
        sendRequest(&request);
        NEXT_REFRESH_TIME2 = millis() + NEXT_REFRESH_PERIOD2;
    }
}

void ConnectivityUtils::refreshData(bool force) {
    if (force || NEXT_REFRESH_TIME < millis()) {
        publishStatus();
        publishTargetTemperature();
        publishTemperatureAndHumidity();
        NEXT_REFRESH_TIME = millis() + NEXT_REFRESH_PERIOD;
    }
}

void ConnectivityUtils::buildTemperatureRequest(request *request) {
    char temperature[6];
    dtostrf(ThermostatData::getInstance().getTemperature(), 5, 2, temperature);
    requestUtils.buildSendOperation(request, temperature, "temperature");
}

void ConnectivityUtils::buildHumidityRequest(request *request) {
    char humidity[6];
    dtostrf(ThermostatData::getInstance().getHumidity(), 5, 2, humidity);
    requestUtils.buildSendOperation(request, humidity, "humidity");
}

void ConnectivityUtils::buildTargetTemperatureRequest(request *request) {
    char temperature[6];
    dtostrf(ThermostatData::getInstance().getTargetTemp(), 5, 2, temperature);
    requestUtils.buildSendOperation(request, temperature, "targetTemp");
}

void ConnectivityUtils::buildAvailability(request *request) {
    requestUtils.buildSendOperation(request, "online", "available");
}

void ConnectivityUtils::buildMode(request *request) {
    requestUtils.buildSendOperation(request, ThermostatData::getInstance().getMode(), "mode");
}

void ConnectivityUtils::buildAction(request *request) {
    requestUtils.buildSendOperation(request, ThermostatData::getInstance().getAction(), "action");
}

void ConnectivityUtils::sendRequest(request *request) {
    uint8_t serializedBuffer[ESPNOW_BUFFERSIZE];
    int serializedLen = requestUtils.serialize(serializedBuffer, request);
    espNowService.send(gatewayAddress, serializedBuffer, serializedLen);
}