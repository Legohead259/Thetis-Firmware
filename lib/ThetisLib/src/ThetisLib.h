#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "0.1.3"

#include <Arduino.h>


// ========================
// === STATUS FUNCTIONS ===
// ========================


/*
Status Table:
State           |  Color  |  Indication  |  Description
----------------|---------|--------------|---------------
Booting         |  PURPLE |   Pulsing    | Thetis is booting, most likely waiting for a WiFi connection
Logging, No GPS |  BLUE   |    Solid     | Thetis is logging, but does not have a GPS fix
Logging, GPS    |  GREEN  |    Solid     | Thetis is logging with a GPS fix
Ready, No GPS   |  BLUE   |   Pulsing    | Accelerometer is calibrated but no GPS fix
Ready, GPS      |  GREEN  |   Pulsing    | Accelerometer is calibrated and there is a GPS fix
Standby         |  AMBER  |    Solid     | Accelerometer is not calibrated yet
*/

typedef enum Status {
    BOOTING,
    LOGGING_NO_GPS,
    LOGGING_GPS,
    READY_NO_GPS,
    READY_GPS,
    STANDBY
} Status_t;


// =============================
// === TELEMETETRY FUNCTIONS ===
// =============================


typedef struct {
    time_t epoch;               // Timestamp in seconds since UNIX epoch (January 1, 1970)
    long curmSecond;            // Current fraction of second of reading
    float voltage;              // Battery voltage in V
    bool GPSFix;                // If GPS has positive fix on location
    uint8_t numSats;            // Number of satellites GPS is communicating with
    uint8_t HDOP;               // Accuracy of GPS reading. Lower is better. In tenths (divide by 10. when displaying)
    long latitude;              // In millionths of a degree (divide by 1000000. when displaying)
    long longitude;             // In millionths of a degree (divide by 1000000. when displaying)
    long GPSSpeed;              // In thousandths of a knot (divide by 1000. when displaying)
    long GPSCourse;             // In thousandths of a degree (divide by 1000. when displaying)
    uint8_t sysCal = 0;         // IMU system calibration, 0-3 with 3 being fully calibrated
    uint8_t gyroCal = 0;        // IMU gyroscope calibration, 0-3 with 3 being fully calibrated
    uint8_t accelCal = 0;       // IMU accelerometer calibration, 0-3 with 3 being fully calibrated
    uint8_t magCal = 0;         // IMU magnetometer calibration, 0-3 with 3 being fully calibrated
    float accelX;               // m/s^2
    float accelY;               // m/s^2
    float accelZ;               // m/s^2
    float gyroX;                // rad/s
    float gyroY;                // rad/s
    float gyroZ;                // rad/s
    float magX;                 // mGauss
    float magY;                 // mGauss
    float magZ;                 // mGauss
    float roll;                 // degrees
    float pitch;                // degrees
    float yaw;                  // degrees
    float linAccelX;            // m/s^2
    float linAccelY;            // m/s^2
    float linAccelZ;            // m/s^2
    float quatW;                //
    float quatX;                //
    float quatY;                //
    float quatZ;                //
    float imuTemp;              // °Celsius from the IMU
    Status_t state;              // State reported by the package.
    uint8_t packetSize;         // The size of the telemetry packet
} telemetry_t;


// ============================
// === FILESYSTEM FUNCTIONS ===
// ============================


#include <filesystem/ThetisFS.h>
#include <filesystem/ThetisConfig.h>


// ===========================
// === TIMESTAMP FUNCTIONS ===
// ===========================


#include <TimeLib.h>
extern tmElements_t timeElements;
void getISO8601Time_RTC(char *buf);


// ========================
// === SENSOR FUNCTIONS ===
// ========================


#include <sensors/Thetis_BNO055.h>
#include <sensors/Thetis_LSM6DSO32.h> 


// =======================
// === RADIO FUNCTIONS ===
// =======================


#include <radios/Thetis_GPS.h>


// ==========================
// === NEOPIXEL FUNCTIONS ===
// ==========================


#include <misc/Thetis_NeoPixel.h>


// =========================
// === LOGGING FUNCTIONS ===
// =========================


#define LOG_BTN_HOLD_TIME 500 // ms

extern long logButtonPresses;
extern long logButtonStartTime;
// extern bool logButtonPressed;
extern bool isLogging;

void IRAM_ATTR logButtonISR();

#endif // THETISLIB_H