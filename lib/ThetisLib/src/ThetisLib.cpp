#include <ThetisLib.h>

// =========================
// === GENERAL FUNCTIONS ===
// =========================


tmElements_t timeElements;

void getISO8601Time_RTC(char *buf) {
    breakTime(now(), timeElements);
    static long _lastSecond = 0;
    static long _lastMSecond = 0;
    long curMSecond = millis();
    if (timeElements.Second == _lastSecond) {
        curMSecond = millis() - _lastMSecond;
        // Serial.println((int) curMSecond); //DEBUG
    }
    else {
        _lastSecond = timeElements.Second;
        _lastMSecond = millis();
        curMSecond = 0;
    }

    // Format timestamp into ISO8601 format
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", timeElements.Year+1970, timeElements.Month, timeElements.Day, timeElements.Hour, timeElements.Minute, timeElements.Second, curMSecond);
}


// =========================
// === LOGGING FUNCTIONS ===
// =========================

long logButtonStartTime = 0;
long logButtonPresses = 0;

void IRAM_ATTR logButtonISR() {
    logButtonPresses++;
    logButtonStartTime = millis();
}