/**
 * @file Thetis Firmware Variant A - Embedded Accelerometer
 * @version 0.2.2
 * 
 * @brief This firmware variant enables Thetis to act as an embedded accelerometer.
 * Accelerometer values will be logged to the onboard storage device and can be accesible via the WiFi interface, if enabled.
 * This firmware will work on a barebones Thetis that does not have a GPS module.
 * Currently compatible with the following boards:
 *      - RevF4 with LSM6DSO32 IMU
 * 
 * Version 0.1.0 - Initial baseline release
 * Version 0.2.0 - Added GPS integration
 * Version 0.2.1 - Integrated internal RTC for timestamps
 * Version 0.2.2 - Fixed log file write issue and implemented log enable functionality
 * 
 * @author Braidan Duffy
 * @date June 10, 2022
 *       July 01, 2022 (last edit)
**/
#include <ThetisLib.h>

#define GPS_SYNC_INTERVAL 1 // minutes
#define GPS_TIMEOUT 5000 // ms
#define IMU_POLL_RATE 52.0 // Hz
#define IMU_POLL_INTERVAL 1000/IMU_POLL_RATE // ms

telemetry_t data;
bool isIMUAvailable = false;
bool DEBUG_MODE = false;
char filename[13];
char timestamp[32];

// Flags
bool isGPSEnable = true;
bool isLogging = false;
bool isLogFileCreated = false;

void syncInternalClockGPS();
bool getTime(const char *str);
bool getDate(const char *str);

void setup() {
    DEBUG_MODE = digitalRead(USB_DETECT); // Check if USB is plugged in
    if (DEBUG_MODE) {
        Serial.begin(115200);
        while(!Serial); // Wait for serial connection
    }

    Serial.println("-------------------------------------------------------");
    Serial.println("   Thetis Firmware Variant A: Embedded Accelerometer   ");
    Serial.println("-------------------------------------------------------");
    Serial.println();

    initNeoPixel();

    isIMUAvailable = initDSO32();
    if (!isIMUAvailable) { // Check IMU initialization
        while(true) blinkCode(IMU_ERROR_CODE); // Block further code execution
    }

    if (!initGPS()) { // Initialize GPS and check if good
        while(true) blinkCode(GPS_ERROR_CODE); // Block further code execution
    }
    MicroNMEA::sendSentence(GPS, "$PMTK220,1000"); // Set GPS update rate to 1000 ms (1 Hz)
    
    if (!initSDCard()) { // Initialize filesystem and check if good
        while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
    }

    attachInterrupt(LOG_EN, logButtonISR, FALLING);

    // TODO: Initialize internal RTC using compile __DATE__ and __TIME__

    // TODO: Load configuration data from file on SD card

    if (isGPSEnable) {
        syncInternalClockGPS();
    }

    // TODO: Begin WiFi station and server, if enabled
}

void loop() {
    static long _lastGPSSync = millis();
    if (isGPSEnable && millis() >= _lastGPSSync+GPS_SYNC_INTERVAL*60000) { // Check GPS enabled and if GPS_SYNC_INTERVAL time has passed
        syncInternalClockGPS();
        _lastGPSSync = millis(); // Reset GPS sync timer flag
    }

    static long _lastIMUPoll = millis();
    if (millis() >= _lastIMUPoll+IMU_POLL_INTERVAL) { // Check if IMU_POLL_INTERVAL time has passed
        pollDSO32();

        // Write data to log file
        if (isLogging) { // TODO: Find the bug when logging with the ISO8601 timestamp. - Could be accessing now() so often? Change to call breakTime every new second???
            char _writeBuf[64];
            getISO8601Time_RTC(timestamp);
            sprintf(_writeBuf, "%s,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n",  
                    timestamp,
                    accel.acceleration.x, accel.acceleration.y, accel.acceleration.z,
                    linAccel.x, linAccel.y, linAccel.z);
            appendFile(SD, filename, _writeBuf);
        }

        _lastIMUPoll = millis(); // Reset IMU poll timer
    }

    // TODO: Implement server refresh at 1 Hz, if client connected

    static long _oldButtonPresses = 0;
    if (logButtonPresses != _oldButtonPresses && !digitalRead(LOG_EN) && millis() >= logButtonStartTime+LOG_BTN_HOLD_TIME) {
        isLogging = !isLogging;
        if (!isLogFileCreated) {
            if (!initLogFile(SD, filename, "Timestamp (ISO 8601),ax,ay,az,lin_ax,lin_ay,lin_az\n")) { // Initialize log file and check if good
                while(true) blinkCode(FILE_ERROR_CODE); // block further code execution
            }
            isLogFileCreated = true;
        }
        digitalWrite(LED_BUILTIN, isLogging);
        _oldButtonPresses = logButtonPresses;
    }
}

void syncInternalClockGPS() {
    Serial.println();
    Serial.print("Attempting to sync internal clock to GPS time...");
    long timeoutStart = millis();
    while(!GPS) { // Wait for a GPS message to arrive
        if (millis() >= timeoutStart+GPS_TIMEOUT) return; // Stop attempt, if TIMEOUT occurs
    }

    while(GPS.available()) { // Check for an available GPS message
        char c = GPS.read();
        Serial.print(c); // Debug
        nmea.process(c);
    }
    if (nmea.isValid()) { // If the GPS has a good fix, reset the internal clock to the GPS time
        tm.Year = nmea.getYear()-1970;
        tm.Month = nmea.getMonth();
        tm.Day = nmea.getDay();
        tm.Hour = nmea.getHour();
        tm.Minute = nmea.getMinute();
        tm.Second = nmea.getSecond();

        setTime(makeTime(tm)); // Reset internal clock
        Serial.println("Done!");
    }
    else {
        Serial.println("GPS fix was not valid - did not sync");
    }
    Serial.println();
}