#include "ConnectivityUtils.hpp"

unsigned long NEXT_REFRESH_TIME_FOR_REFRESH_DATA = millis();
unsigned long NEXT_REFRESH_PERIOD_FOR_REFRESH_DATA = 300000;

unsigned long NEXT_REFRESH_TIME_FOR_CHECK_TOPICS = millis();
unsigned long NEXT_REFRESH_PERIOD_FOR_CHECK_TOPICS = 1000;

RequestUtils requestUtils = RequestUtils::getInstance();
ResponseUtils responseUtils = ResponseUtils::getInstance();

char lastStatusMessage[6] = "none";

void manageSendResult(int result) {
    if (result == 0 && strcmp(lastStatusMessage, "true") != 0) {
        strcpy(lastStatusMessage, "true");
        EventQueue::getInstance().addEvent(EVENT_TYPES::MESSAGE_OK);
    } else if (result != 0 && strcmp(lastStatusMessage, "false") != 0) {
        strcpy(lastStatusMessage, "false");
        EventQueue::getInstance().addEvent(EVENT_TYPES::MESSAGE_FAILED);
    }
}

void opResponseHandler(response *response, response_OpResponse *opResponse, int operationNumber) {
    if (response->message_type == 631 && opResponse->result_code == 0) {
        if (operationNumber == 0) {
            ThermostatData::getInstance().changeMode(opResponse->payload);
            EventQueue::getInstance().addEvent(EVENT_TYPES::MODE);
        } else {
            ThermostatData::getInstance().setTargetTemp(atof(opResponse->payload));
            EventQueue::getInstance().addEvent(EVENT_TYPES::TARGET_TEMPERATURE);
        }
    }
}

void espNowSendCallBack(const uint8_t *mac_addr, esp_now_send_status_t status) {
    logTrace("Send message status:\t");
    logTraceln(status == 0 ? "Sent Successfully" : "Sent Failed");
    manageSendResult(status);
}

void espNowRecvCallBack(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    responseUtils.manage(data, data_len, responseHandlerDummy, opResponseHandler);
}

ConnectivityUtils::ConnectivityUtils(const char *clientName, uint8_t *gatewayAddress, uint8_t *clientAdress) {
    this->clientName = clientName;
    memcpy(this->gatewayAddress, gatewayAddress, 6);
    memcpy(this->clientAdress, clientAdress, 6);
}

void ConnectivityUtils::setupConnectivity() {
    if (ThermostatData::getInstance().isConnectivityActive()) {
        setupWiFiForEspNow();
        espNowService.setup(espNowSendCallBack, espNowRecvCallBack);
        refreshData(true);
    }
    strcpy(lastStatusMessage, "none");
}

void ConnectivityUtils::disconnect() {
    publishStatus(true);
    delay(500);
    WiFi.mode(WIFI_OFF);
    strcpy(lastStatusMessage, "none");
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

void ConnectivityUtils::publishStatus(bool ignoreConectivityStatus) {
    request request = requestUtils.createRequest(clientName, clientAdress, 1);
    buildAvailability(&request, ThermostatData::getInstance().isConnectivityActive() ? "online" : "offline");
    buildMode(&request);
    buildAction(&request);
    sendRequest(&request, ignoreConectivityStatus);
}

void ConnectivityUtils::checkTopics() {
    if (NEXT_REFRESH_TIME_FOR_CHECK_TOPICS < millis()) {
        request request = requestUtils.createRequest(clientName, clientAdress, 631);
        requestUtils.buildSubscribeOperation(&request, "chg/mode");
        requestUtils.buildSubscribeOperation(&request, "chg/tgTemp");
        sendRequest(&request);
        NEXT_REFRESH_TIME_FOR_CHECK_TOPICS = millis() + NEXT_REFRESH_PERIOD_FOR_CHECK_TOPICS;
    }
}

void ConnectivityUtils::refreshData(bool force) {
    if (force || NEXT_REFRESH_TIME_FOR_REFRESH_DATA < millis()) {
        publishStatus();
        publishTargetTemperature();
        publishTemperatureAndHumidity();
        NEXT_REFRESH_TIME_FOR_REFRESH_DATA = millis() + NEXT_REFRESH_PERIOD_FOR_REFRESH_DATA;
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

void ConnectivityUtils::buildAvailability(request *request, const char *status) {
    requestUtils.buildSendOperation(request, status, "available");
}

void ConnectivityUtils::buildMode(request *request) {
    requestUtils.buildSendOperation(request, ThermostatData::getInstance().getMode(), "mode");
}

void ConnectivityUtils::buildAction(request *request) {
    requestUtils.buildSendOperation(request, ThermostatData::getInstance().getAction(), "action");
}

void ConnectivityUtils::sendRequest(request *request, bool ignoreConectivityStatus) {
    if (ignoreConectivityStatus || ThermostatData::getInstance().isConnectivityActive()) {
        int result = espNowService.sendRequest(gatewayAddress, request);
        manageSendResult(result);
    }
}