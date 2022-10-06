/**
 * @file Thetis Firmware
 * @version 1.0.0
 * 
 * @brief 
 * 
 * Version 0.1.0 - Initial baseline release
 * Version 0.2.0 - Added GPS integration
 * Version 0.2.1 - Integrated internal RTC for timestamps
 * Version 0.2.2 - Fixed log file write issue and implemented log enable functionality
 * Version 0.2.3 - Fixed major bug with the device crashing on log enabled
 * Version 0.3.0 - Added loading configurations from file on SPIFFS; added WiFi hotspot functionality
 * Version 1.0.0 - Complete codebase refactor to make development easier and streamline multiple things; ThetisLib will be deprecated
 * 
 * @author Braidan Duffy
 * @date June 10, 2022
 *       Ocotober 4, 2022 (last edit)
**/
#include <ThetisLib.h>


// Flags
bool isDebugging = false;
bool isIMUAvailable = false;
bool isLogging = false;
bool isLogFileCreated = false;
bool isIMUCalibrated = true;

void setup() {
    isDebugging = digitalRead(USB_DETECT); // Check if USB is plugged in
    if (isDebugging) {
        Serial.begin(115200);
        while(!Serial); // Wait for serial connection
    }

    Serial.println("-------------------------------------");
    Serial.println("    Thetis Firmware Version 1.0.0    ");
    Serial.println("-------------------------------------");
    Serial.println();

    if (!initNeoPixel()) { // Initialize the NeoPixel
        while(true); // Block further code execution
    }

    if (!initDSO32()) { // Check IMU initialization
        while(true) blinkCode(IMU_ERROR_CODE); // Block further code execution
    }

    if (!initGPS()) { // Initialize GPS and check if good
        while(true) blinkCode(GPS_ERROR_CODE); // Block further code execution
    }
    
    if (!initSDCard()) { // Initialize SD card filesystem and check if good
        while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
    }

    if (!initSPIFFS()) { // Initialize SD card filesystem and check if good
        while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
    }

    if (!config.begin("/config.cfg", 127)) { // Initialize config file and check if good
        while (true) blinkCode(FILE_ERROR_CODE); // Block code execution
    }
    config.loadConfigurations(); // Load in configuration data from the file
   
    syncInternalClockGPS(); // Attempt to sync internal clock to GPS, if it has a fix already

    #ifdef WIFI_AP_ENABLE
    if (!initWIFI_AP()) { // Start WIFI Access Point
        while (true) blinkCode(RADIO_ERROR_CODE); // Block code execution
    } 
    #endif

    // Attach the log enable button interrupt
    attachInterrupt(LOG_EN, logButtonISR, FALLING);
}

void loop() {
    // State and LED updates
    updateSystemState();
    updateSystemLED();

    static long _lastGPSSync = millis();
    if (configData.gpsEnable && millis() >= _lastGPSSync+GPS_SYNC_INTERVAL*60000) { // Check GPS enabled and if GPS_SYNC_INTERVAL time has passed
        syncInternalClockGPS();
        _lastGPSSync = millis(); // Reset GPS sync timer flag
    }

    // static long _lastIMUPoll = millis();
    // if (millis() >= _lastIMUPoll+IMU_POLL_INTERVAL) { // Check if IMU_POLL_INTERVAL time has passed
    //     pollDSO32();

    //     // Write data to log file
    //     if (isLogging) {
    //         char _writeBuf[128];
    //         getISO8601Time_RTC(timestamp);
    //         sprintf(_writeBuf, "%s,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n",  
    //                 timestamp,
    //                 accel.acceleration.x, accel.acceleration.y, accel.acceleration.z,
    //                 linAccel.x, linAccel.y, linAccel.z);
    //         if (!appendFile(SD, filename, _writeBuf)) { // Check if file append was successful; enter error state if not
    //             while(true) blinkCode(FILE_ERROR_CODE);
    //         }
    //     }

    //     _lastIMUPoll = millis(); // Reset IMU poll timer
    // }

    // Logging handler
    static uint8_t _oldButtonPresses = 0;
    if (logButtonPresses != _oldButtonPresses && !digitalRead(LOG_EN) && millis() >= logButtonStartTime+LOG_BTN_HOLD_TIME) { // Check if BTN0 has been pressed and has been held for sufficient time
        isLogging = !isLogging;
        if (!isLogFileCreated) {
            if (!initTelemetryLogFile(SD)) { // Initialize log file and check if good
                while(true) blinkCode(FILE_ERROR_CODE); // block further code execution
            }
            isLogFileCreated = true;
        }
        digitalWrite(LED_BUILTIN, isLogging);
        _oldButtonPresses = logButtonPresses;
    }
}