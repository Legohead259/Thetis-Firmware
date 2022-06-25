/**
 * @file Thetis Firmware Variant A - Embedded Accelerometer
 * @version 0.2.0
 * 
 * @brief This firmware variant emables Thetis to act as an embedded accelerometer.
 * Accelerometer values will be logged to the onboard storage device and can be accesible via the WiFi interface, if enabled.
 * This firmware will work on a barebones Thetis that does not have a GPS module.
 * Currently compatible with the following boards:
 *      - RevF4 with LSM6DSO32 IMU
 * 
 * Version 0.1.0 - Initial baseline release
 * Version 0.2.0 - Added GPS integration
 * 
 * @author Braidan Duffy
 * @date June 10, 2022
 *       June 21, 2022 (last edit)
**/
#include <ThetisLib.h>

telemetry_t data;
bool isIMUAvailable = false;
bool DEBUG_MODE = false;
char filename[13];
char timestamp[32];

// Flags
bool IS_GPS_ENABLE = true;

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

    if (!initLogFile(SD, filename)) { // Initialize log file and check if good
        while(true) blinkCode(FILE_ERROR_CODE); // block further code execution
    }
    // Write header for the log file
    writeFile(SD, filename, "Timestamp (ISO 8601), ax, ay, az, lin_ax, lin_ay, lin_az");

    // TODO: Load configuration data from file on SD card

    // TODO: Wait for GPS message, then synchronize internal RTC to it, if enabled

    // TODO: Begin WiFi station and server, if enabled
}

void loop() {
    // TODO: Implement GPS polling
    if (IS_GPS_ENABLE) {
        while (GPS.available()) { // Read and parse GPS messages when they are available
            char c = GPS.read();
            Serial.print(c);
            nmea.process(c);
	    }
        getISO8601Time(timestamp, millis());
    }
    // TODO: Implement asynchronous polling
    pollDSO32();

    // Write data to log file
    char _writeBuf[64];
    sprintf(_writeBuf, "%d,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f",  
            timestamp, // TODO: update to internal RTC value (or GPS)
            accel.acceleration.x, accel.acceleration.y, accel.acceleration.z,
            linAccel.x, linAccel.y, linAccel.z);
    appendFile(SD, filename, _writeBuf);

    // TODO: Implement server refresh at 1 Hz, if client connected

    delay(1000/52);
}