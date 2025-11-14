#ifndef __COMMONS_HPP
#define __COMMONS_HPP

#include "Arduino.h"

#ifdef TRACE_LOG_FLAG
#define logTrace Serial.print
#define logTraceln Serial.println
#define logTracef Serial.printf
#else
#define logTrace
#define logTraceln
#define logTracef
#endif

#ifdef DEBUG_LOG_FLAG
#define logDebug Serial.print
#define logDebugln Serial.println
#define logDebugf Serial.printf
#else
#define logDebug
#define logDebugln
#define logDebugf
#endif

#ifdef INFO_LOG_FLAG
#define logInfo Serial.print
#define logInfoln Serial.println
#define logInfof Serial.printf
#else
#define logInfo
#define logInfoln
#define logInfof
#endif

#ifdef ERROR_LOG_FLAG
#define logError Serial.print
#define logErrorln Serial.println
#define logErrorf Serial.printf
#else
#define logError
#define logErrorln
#define logErrorf
#endif

#endif