#include "TftUtils.hpp"

#define TFT_CS 3
#define TFT_DC 2
#define TFT_MOSI 6
#define TFT_CLK 4
#define TFT_RST 1
#define TFT_MISO 5
#define TS_CS 0

/*______Assign pressure_______*/
#define MINPRESSURE 400
#define MAXPRESSURE 2000
/*_______Assigned______*/

/*____Calibrate TFT LCD_____*/
#define TS_MINX 320
#define TS_MINY 490
#define TS_MAXX 3800
#define TS_MAXY 3900
/*______End of Calibration______*/

#define MAX_TEMPERATURE 35
#define MIN_TEMPERATURE 7

TS_Point tsPoint;
bool touchPressed = false;

TftUtils::TftUtils() : tft(TFT_CS, TFT_DC, TFT_RST), touch(TS_CS) {}

void TftUtils::initTft() {
    tft.begin();
    tft.setRotation(2);
    touch.begin();
    touch.setRotation(2);
    drawMainScreen();
}

void TftUtils::drawMainScreen() {
    tft.fillScreen(ILI9341_BLACK);
    updateCircleColor();
    drawUpDownButton();
    drawPowerButton();
    drawWiFiButton(ILI9341_ULTRA_DARKGREY);
    updateTargetTemp();
}

void TftUtils::updateCircleColor() {
    logDebugln("TftUtils.hpp\t\t\tDraw circles color.");
    // draw big circle
    unsigned char i;
    if (strcmp(ThermostatData::getInstance().getAction(), "heating") == 0) {
        for (i = 0; i < 10; i++)
            tft.drawCircle(120, 120, 80 + i, ILI9341_RED);
    } else if (strcmp(ThermostatData::getInstance().getAction(), "idle") == 0) {
        for (i = 0; i < 10; i++)
            tft.drawCircle(120, 120, 80 + i, ILI9341_GREEN);
    } else {
        for (i = 0; i < 10; i++)
            tft.drawCircle(120, 120, 80 + i, ILI9341_ULTRA_DARKGREY);
    }

    // draw small
    tft.fillCircle(60, 200, 45, ILI9341_ULTRA_DARKGREY);
    updateRoomTemp();
    // draw °C in big circle
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setFont(&FreeSansBold9pt7b);
    tft.setCursor(143, 100);
    tft.print("o");
    tft.setFont(&FreeSansBold24pt7b);
    tft.setCursor(153, 130);
    tft.print("C");

    // draw room and °C in small circle
    tft.setTextColor(ILI9341_WHITE, ILI9341_ULTRA_DARKGREY);
    tft.setFont(&FreeSansBold12pt7b);
    tft.setCursor(79, 219);
    tft.print("C");
    tft.drawCircle(73, 203, 2, ILI9341_WHITE);
    tft.drawCircle(73, 203, 3, ILI9341_WHITE);
    tft.setFont(&FreeSansBold9pt7b);
    tft.setCursor(33, 190);
    tft.print("Room");
    updateTargetTemp();
}

void TftUtils::updateTargetTemp() {
    logDebugln("TftUtils.hpp\t\t\tDraw target temperature.");
    int16_t x1, y1;
    uint16_t w, h;
    char targetTempAux[5];
    dtostrf(ThermostatData::getInstance().getTargetTemp(), 4, 1, targetTempAux);
    tft.fillRect(50, 96, 93, 50, ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setFont(&FreeSansBold24pt7b);
    tft.getTextBounds(targetTempAux, 53, 130, &x1, &y1, &w, &h);
    tft.setCursor(137 - w, 130);
    tft.print(targetTempAux);
}

void TftUtils::updateRoomTemp() {
    logDebugln("TftUtils.hpp\t\t\tDraw room temperature.");
    int16_t x1, y1;
    uint16_t w, h;
    char currentValue[5];
    dtostrf(ThermostatData::getInstance().getTemperature(), 4, 1, currentValue);
    tft.fillRect(20, 200, 50, 21, ILI9341_ULTRA_DARKGREY);
    tft.setTextColor(ILI9341_WHITE, ILI9341_ULTRA_DARKGREY);
    tft.setFont(&FreeSansBold12pt7b);
    tft.getTextBounds(currentValue, 40, 219, &x1, &y1, &w, &h);
    tft.setCursor(66 - w, 219);
    tft.print(currentValue);
}

void TftUtils::drawUpDownButton() {
    // up button
    tft.fillTriangle(215, 10, 230, 30, 200, 30, ILI9341_WHITE);
    // down button
    tft.fillTriangle(215, 230, 230, 210, 200, 210, ILI9341_WHITE);
}

void TftUtils::drawPowerButton() {
    logDebugln("TftUtils.hpp\t\t\tDraw power button");
    if (strcmp(ThermostatData::getInstance().getMode(), "heat") == 0) {
        tft.drawBitmap(100, 275, powerIcon, 40, 40, ILI9341_WHITE);
    } else {
        tft.drawBitmap(100, 275, powerIcon, 40, 40, ILI9341_RED);
    }
}

void TftUtils::drawWiFiButton(int wifiButtonColor) {
    logDebugln("TftUtils.hpp\t\t\tDraw WiFi button.");
    tft.drawBitmap(10, 10, wifiIcon, 24, 24, wifiButtonColor);
}

void TftUtils::detectToutch() {
    if (isTouchEvent()) {
        int x = tsPoint.x;
        int y = tsPoint.y;
        if (touchPressed == false) {
            detectButtons(x, y);
        }
        touchPressed = true;
    } else {
        touchPressed = false;
    }
}

bool TftUtils::isTouchEvent() {
    tsPoint = touch.getPoint();
    if (tsPoint.z > MINPRESSURE) {
        tsPoint.x = map(tsPoint.x, TS_MINX, TS_MAXX, 0, 240);
        tsPoint.y = map(tsPoint.y, TS_MINY, TS_MAXY, 0, 320);
        return true;
    }
    return false;
}

void TftUtils::detectButtons(int x, int y) {
    // up down temperature buttons
    if (x >= 190) {
        if (y <= 40) {
            if (ThermostatData::getInstance().getTargetTemp() < MAX_TEMPERATURE) {
                ThermostatData::getInstance().increaseTargetTemp();
                EventQueue::getInstance().addEvent(EVENT_TYPES::TARGET_TEMPERATURE);
            }
        } else if (y >= 200 && y <= 260) {
            if (ThermostatData::getInstance().getTargetTemp() > MIN_TEMPERATURE) {
                ThermostatData::getInstance().decreaseTargetTemp();
                EventQueue::getInstance().addEvent(EVENT_TYPES::TARGET_TEMPERATURE);
            }
        }
    }
    // power button
    else if (x >= 95 && x <= 145 && y >= 270) {
        if (strcmp(ThermostatData::getInstance().getMode(), "heat") == 0) {
            ThermostatData::getInstance().changeMode("off");
        } else {
            ThermostatData::getInstance().changeMode("heat");
        }
        EventQueue::getInstance().addEvent(EVENT_TYPES::MODE);
    }
    // wifi button
    else if (x <= 30 && y <= 30) {
        ThermostatData::getInstance().toggleConnectivity();
        EventQueue::getInstance().addEvent(EVENT_TYPES::CONNECTIVITY);
    }
}