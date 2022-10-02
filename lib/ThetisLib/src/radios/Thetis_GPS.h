#ifndef THETIS_GPS_H
#define THETIS_GPS_H

// =====================
// === GPS FUNCTIONS ===
// =====================


#include <utility/MicroNMEA.h>

// TODO: Implement MTK3339 API
// TODO: Make GPS object-oriented

typedef enum {
    BAUD_9600 = 9600,
    BAUD_38400 = 38400
} gps_baudrates_t;

typedef enum {
    REFRESH_RATE_CMD = 220,
    BAUDRATE_CMD = 251,
    SENTANCE_TYPES_CMD = 14
} gps_commands_t;

extern HardwareSerial& GPS;
extern char nmeaBuffer[100];
extern MicroNMEA nmea;
extern bool ledState;
extern volatile bool ppsTriggered;

bool initGPS();

void getISO8601Time(char *buf);

#endif // ndef THETIS_GPS_H