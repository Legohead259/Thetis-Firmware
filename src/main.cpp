/**
 * @file Thetis Firmware
 * @version 1.1.0
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
 * Version 1.1.0 - Optimizing logging process for increased sample rate
 * 
 * @author Braidan Duffy
 * @date June 10, 2022
 *       November 4, 2022 (last edit)
**/
#include <ThetisLib.h>

#define logInterval 1000/32.0f // sample at 32 Hz

// Flags
bool isDebugging = false;
bool isIMUAvailable = false;
bool isLogging = false;
bool isLogFileCreated = false;
bool isIMUCalibrated = true;

void setup() {
    // Casting to int is important as just uint8_t types will invoke the "slave" begin, not the master
    Wire.begin((int) SDA, (int) SCL);

    isDebugging = digitalRead(USB_DETECT); // Check if USB is plugged in
    if (isDebugging) {
        Serial.begin(115200);
        while(!Serial); // Wait for serial connection
    }

    Serial.println("-------------------------------------");
    Serial.println("    Thetis Firmware Version 1.1.0    ");
    Serial.println("-------------------------------------");
    Serial.println();

    if (!initNeoPixel()) { // Initialize the NeoPixel
        while(true); // Block further code execution
    }

    if (!initDSO32()) { // Check IMU initialization
        while(true) blinkCode(IMU_ERROR_CODE); // Block further code execution
    }
    
    initFusion(); // Initialize the sensor fusion algorithms

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

    // Poll GPS and update data structure
    static unsigned long _lastGPSPoll = millis();
    if ((millis() - _lastGPSPoll) >= GPS_POLL_INTERVAL) { // Check if GPS_POLL_INTERVAL has passed
        pollGPS();
        _lastGPSPoll = millis(); // Reset GPS poll timer
    }

    // Timestamp synchronization
    static unsigned long _lastGPSSync = millis();
    if ((millis() - _lastGPSSync) >= GPS_SYNC_INTERVAL*60000) { // Check if GPS_SYNC_INTERVAL has passed
        syncInternalClockGPS();
        _lastGPSSync = millis(); // Reset GPS sync timer
    }

    // Update sensor fusion algorithm and data structure
    static unsigned long _lastIMUPoll = millis();
    if ((millis() - _lastIMUPoll) >= imuPollInterval) { // Check if IMU_POLL_INTERVAL time has passed
        unsigned long _fusionStartTime = millis();
        updateFusion();

        Serial.printf("Time to process sensor fusion: %d ms\r\n", millis() - _fusionStartTime); // DEBUG
        _lastIMUPoll = millis(); // Reset IMU poll timer
    }

    // Write data to log file
    static unsigned long _lastLogTime = millis();
    if (isLogging && (millis() - _lastLogTime) >= logInterval) {
        unsigned long _logStartTime = millis();
        if (!logDataBin(SD)) {
            // TODO: Figure out a better way to handle this type of error
            while (true) blinkCode(FILE_ERROR_CODE); // Block further code execution
        }
        Serial.printf("Time to log data: %d ms\r\n", millis() - _logStartTime); // DEBUG
        _lastLogTime = millis(); // Reset log timer flag
    }

    // Logging handler
    static uint8_t _oldButtonPresses = 0;
    if (logButtonPresses != _oldButtonPresses && !digitalRead(LOG_EN) && millis() >= logButtonStartTime+LOG_BTN_HOLD_TIME) { // Check if BTN0 has been pressed and has been held for sufficient time
        isLogging = !isLogging;
        if (!isLogFileCreated) {
            if (!initDataLogFile(SD)) { // Initialize log file and check if good
                while(true) blinkCode(FILE_ERROR_CODE); // block further code execution
            }
            isLogFileCreated = true;
        }
        digitalWrite(LED_BUILTIN, isLogging);
        _oldButtonPresses = logButtonPresses;
    }
}