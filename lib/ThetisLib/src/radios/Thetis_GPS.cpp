#include "Thetis_GPS.h"

// ============================
// === GPS MODULE FUNCTIONS ===
// ============================


HardwareSerial& GPS = GPS_SERIAL_PORT;
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
bool ledState = LOW;
volatile bool ppsTriggered = false;

bool initGPS() {
    #ifdef GPS_DEBUG
    DEBUG_SERIAL_PORT.print("Initializing GPS..."); // DEBUG
    #endif
    GPS.begin(9600); // Begin talking with GPS at default 9600 baud.
    // TODO: Automatically determine GPS initial baudrate
    if (!GPS) {
        #ifdef GPS_DEBUG
        DEBUG_SERIAL_PORT.println("Failed to initialize GPS"); // DEBUG
        #endif
        return false;
    }
    delay(10);
    MicroNMEA::sendSentence(GPS, "$PMTK251,38400");         // Set GPS baudrate to 38400
    delay(10);
    GPS.begin(38400);
    MicroNMEA::sendSentence(GPS, "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0"); // Enable NMEA RMC and GGA sentences
    MicroNMEA::sendSentence(GPS, "$PMTK220,100");           // Set GPS update rate to 100 ms (10 Hz)

    // TODO: implement a check for good GPS data
    #ifdef GPS_DEBUG
    DEBUG_SERIAL_PORT.println("done!"); // DEBUG
    #endif
    return true;
}

// TODO: Implement MTK3339 API 

// void changeBaudrate(gps_baudrates_t baud) {
//     sendCommand(BAUDRATE_CMD, )
// }

// void sendCommand(gps_commands_t cmd, char *payload[], HardwareSerial &gps, Stream &out) {
//     char _buf[64];
//     sprintf(_buf, "$PMTK%d,%s", cmd, payload);
//     MicroNMEA::sendSentence(gps, _buf);
//     out.printf("Sending: %s\n\r", _buf);
// }

// =========================
// === WRAPPER FUNCTIONS ===
// =========================


void getISO8601Time(char *buf) {
    static long _oldMillis = 0;
    static uint8_t _oldSecond = 0;
    if (nmea.getSecond() != _oldSecond) { // One second has passed, reset millis offset and second
        _oldSecond = nmea.getSecond();
        _oldMillis = millis();
    }
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",nmea.getYear(), nmea.getMonth(), nmea.getDay(), nmea.getHour(), nmea.getMinute(), nmea.getSecond(), millis()-_oldMillis);
}


// =========================
// === HANDLER FUNCTIONS ===
// =========================


void ppsHandler(void) {
    ppsTriggered = true;
}

void printUnknownSentence(MicroNMEA &nmea) {
    DEBUG_SERIAL_PORT.println();
	DEBUG_SERIAL_PORT.print("Unknown sentence: ");
	DEBUG_SERIAL_PORT.println(nmea.getSentence());
}