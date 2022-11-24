/**
 * @file Thetis Firmware
 * @version 1.1.1
 * 
 * @brief 
 * 
 * Version 0.1.0 - Initial baseline release
 * Version 0.2.0 - Added GPS integration
 * Version 0.2.1 - Integrated internal RTC for timestamps
 * Version 0.2.2 - Fixed log file write issue and implemented log enable functionality
 * Version 0.2.3 - Fixed major bug with the device crashing on log enabled
 * Version 0.3.0 - Added loading configurations from file on SPIFFS; added WiFi hotspot functionality
 * Version 1.0.0 - Complete codebase refactor to make development easier and streamline multiple things
 * Version 1.1.0 - Optimizing logging process for increased sample rate; added more configuration options
 * Version 1.1.1 - Fixed issue where internal RTC was not syncing to GPS time on startup
 * Version 1.1.2 - Enabled faster logging rates up to ~90 Hz
 * Version 1.1.3 - Massive overhaul to logging system
 * 
 * @author Braidan Duffy
 * @date June 10, 2022
 *       November 4, 2022 (last edit)
**/
#include <ThetisLib.h>

float logFrequency; // Hz 
float logInterval;  // Time between log updates [ms]
unsigned long logButtonPresses;
unsigned long logButtonStartTime;

// Flags
bool isDebugging = false;
bool isIMUAvailable = false;
bool isLogging = false;
bool isLogFileCreated = false;
bool isIMUCalibrated = true;

void updateSettings();
void IRAM_ATTR logButtonISR();

void setup() {
    // Casting to int is important as just uint8_t types will invoke the "slave" begin, not the master
    Wire.begin((int) SDA, (int) SCL);

    isDebugging = digitalRead(USB_DETECT); // Check if USB is plugged in
    if (isDebugging && diagPrintLogger.begin(&Serial, LogLevel::VERBOSE)) {
        Serial.begin(115200);
        while(!Serial); // Wait for serial connection
    }
    
    diagLogger = isDebugging ? &diagPrintLogger : &diagFileLogger;

    Serial.println("-------------------------------------");
    Serial.println("    Thetis Firmware Version 1.1.3    ");
    Serial.println("-------------------------------------");
    Serial.println();

    if (!initNeoPixel()) { // Initialize the NeoPixel
        while(true); // Block further code execution
    }

    if (!diagFileLogger.begin(SD, SD_CS, LogLevel::DEBUG)) {
        while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
    }

    if (!initSPIFFS()) { // Initialize SD card filesystem and check if good
        while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
    }

    if (!config.begin("/config.cfg", 127)) { // Initialize config file and check if good
        while (true) blinkCode(FILE_ERROR_CODE); // Block code execution
    }
    config.loadConfigurations(); // Load in configuration data from the file
    updateSettings();

    if (!initGPS()) { // Initialize GPS and check if good
        while(true) blinkCode(GPS_ERROR_CODE); // Block further code execution
    }

    pollGPS();
    syncInternalClockGPS(); // Attempt to sync internal clock to GPS, if it has a fix already

    if (!dataLogger.begin(SD, SD_CS)) { // Initialize SD card filesystem and check if good
        while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
    }

    if (!initDSO32()) { // Check IMU initialization
        while(true) blinkCode(IMU_ERROR_CODE); // Block further code execution
    }
    
    initFusion(); // Initialize the sensor fusion algorithms

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
    if ((millis() - _lastIMUPoll) >= fusionUpdateInterval) { // Check if IMU_POLL_INTERVAL time has passed
        unsigned long _fusionStartTime = millis();
        updateFusion();

        diagLogger->verbose("Time to process sensor fusion: %d ms", millis() - _fusionStartTime);
        _lastIMUPoll = millis(); // Reset IMU poll timer
    }

    // Write data to log file
    static unsigned long _lastLogTime = millis();
    if (isLogging && (millis() - _lastLogTime) >= logInterval) {
        unsigned long _logStartTime = micros();
        dataLogger.writeTelemetryData();
        diagLogger->verbose("Time to log data: %d us", micros() - _logStartTime);
        _lastLogTime = millis();
    }

    // Logging handler
    static uint8_t _oldButtonPresses = 0;
    if (logButtonPresses != _oldButtonPresses && !digitalRead(LOG_EN) && millis() >= logButtonStartTime+LOG_BTN_HOLD_TIME) { // Check if BTN0 has been pressed and has been held for sufficient time
        isLogging = !isLogging;
        if (isLogging) {
            dataLogger.start(SD);
        }
        else {
            dataLogger.stop();
        }
        digitalWrite(LED_BUILTIN, isLogging);
        _oldButtonPresses = logButtonPresses;
    }
}

void updateSettings() {
    diagLogger->info("Updating settings...");
    // -----Sensor Configurations-----
    dso32.setAccelRange(getAccelRange(configData.accelRange));
    dso32.setGyroRange(getGyroRange(configData.gyroRange));
    dso32.setAccelDataRate(getDataRate(configData.imuDataRate));
    dso32.setGyroDataRate(getDataRate(configData.imuDataRate));
    // TODO: Add magnetometer (LIS3MDL)
    fusionUpdateInterval = 1000/configData.fusionUpdateRate;

    // -----Logging Configurations-----
    logFrequency = configData.loggingUpdateRate;
    logInterval = 1000/logFrequency;
    isDebugging ? diagLogger->setLogLevel(configData.logPrintLevel) : diagLogger->setLogLevel(configData.logFileLevel);
    diagLogger->info("done!");
}


// ==================================
// === INTERRUPT SERVICE ROUTINES ===
// ==================================


void IRAM_ATTR logButtonISR() {
    logButtonPresses++;
    logButtonStartTime = millis();
}