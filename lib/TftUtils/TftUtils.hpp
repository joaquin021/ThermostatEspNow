#ifndef __TFT_UTILS_HPP
#define __TFT_UTILS_HPP

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <XPT2046_Touchscreen.h>
#include <WiFi.h>

#include "Commons.hpp"
#include "EventQueue.hpp"
#include "ThermostatData.hpp"
#include "usergraphics.h"

#define ILI9341_ULTRA_DARKGREY 0x632C

// WiFi status colors
#define WIFI_COLOR_DISCONNECTED ILI9341_DARKGREY
#define WIFI_COLOR_CONNECTING ILI9341_YELLOW
#define WIFI_COLOR_CONNECTED ILI9341_WHITE

class TftUtils {
   private:
    Adafruit_ILI9341 tft;
    XPT2046_Touchscreen touch;
    void drawMainScreen();
    void drawUpDownButton();
    bool isTouchEvent();
    void detectButtons(int x, int y);

   public:
    TftUtils();
    void initTft();
    void detectToutch();
    void updateTargetTemp();
    void updateRoomTemp();
    void drawPowerButton();
    void updateCircleColor();
    void drawWiFiButton(int wifiButtonColor);
};

#endif