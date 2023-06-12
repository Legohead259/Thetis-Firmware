/**
 * @file main.cpp
 * @version 1.1.3
 * @author Braidan Duffy (bduffy2018@my.fit.edu)
 * @brief 
 * @date June 10, 2022
 *       November 4, 2022 (last edit)
 * 
 * CHANGELOG:
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
 * Version 1.2.0 - Added FTP server - began implementing WiFi functionality
 * Version 1.2.1 - Added asynchronous server for enable/disabling logging over HTTP GET requests
 * Version 1.2.2 - Fixed NeoPixel functionality
 * Version 1.2.3 - Added magnetometer functionality
 * Version 1.2.4 - Added battery gauge
 *               - Changed the way logging interval is handled; it is now user configurable
 *               - Reintroduced RTC timestamp bug fix
 * Version 1.2.5 - Fixed GPS polling issue
 *               - Reverted log polling method
 * Version 1.3.0 - Integrated TimerEvents rework
 *               - Integrated xioAPI (ThetisAPI)
 *               - Integrated Fusion rework
**/
#define __FIRMWARE_VERSION__ "1.3.0"

#include <ThetisLib.h>

unsigned long logFrequency; // Hz 
unsigned long logInterval;  // Time between log updates [ms]
unsigned long logButtonPresses;
unsigned long logButtonStartTime;

// Flags
bool isDebugging = false;
bool isIMUAvailable = false;
bool isLogFileCreated = false;
bool isIMUCalibrated = true;

void updateSettings();
void IRAM_ATTR logButtonISR();
void ARDUINO_ISR_ATTR onTimer();

void setup() {
    // Casting to int is important as just uint8_t types will invoke the "slave" begin, not the master
    Wire.begin((int) SDA, (int) SCL);

    isDebugging = digitalRead(USB_DETECT); // Check if USB is plugged in
    if (isDebugging && diagPrintLogger.begin(&Serial, LogLevel::DEBUG)) {
        Serial.begin(115200);
        while(!Serial); // Wait for serial connection
    }
    
    diagLogger = isDebugging ? &diagPrintLogger : &diagFileLogger;

    Serial.println("-------------------------------------");
    Serial.println("    Thetis Firmware Version 1.3.0    ");
    Serial.println("-------------------------------------");
    Serial.println();

    setSystemState(BOOTING);

    if (!initNeoPixel()) { // Initialize the NeoPixel
        while(true); // Block further code execution
    }

    // if (!digitalRead(SD_CARD_DETECT) || !diagFileLogger.begin(SD, SD_CS, LogLevel::DEBUG)) {
    //     blinkCode((ErrorCode_t) B1010, AMBER); // Flash a warning that SD card not detected or failed to start
    //     if (!diagFileLogger.begin(SD, XTSD_CS, LogLevel::DEBUG)) { // Switch to logging on XTSD card
    //         while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
    //     }
    // }

    if (!initSPIFFS()) { // Initialize SD card filesystem and check if good
        while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
    }

    if (!api.begin(&Serial)) {
        while(true) blinkCode(GEN_ERROR_CODE); // Block further code execution
    }

    if (!loadConfigurationsFromJSON(true, "/config.json")) {
        while(true) blinkCode(FILE_ERROR_CODE);
    }
    thetisSettingsInitialize();

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

    #ifdef MAG_ENABLE
    if (!initLIS3MDL()) { // Check magnetometer initialization
        while(true) blinkCode(IMU_ERROR_CODE);
    }
    #endif // MAG_ENABLE
    
    #if defined(REV_F5) || defined(REV_G2)
    if (!initMAX17048()) {
        while(true) blinkCode(GEN_ERROR_CODE); // Block further code execution
    }
    #endif // defined(REV_F5) || defined(REV_G2)

    #ifdef WIFI_ENABLE
    if (settings.wirelessMode == WIRELESS_AP) { // Start WiFi in Access Point mode
        if (!initWIFIAP()) while (true) blinkCode(RADIO_ERROR_CODE); // Block further code execution
    }

    if (settings.wirelessMode == WIRELESS_CLIENT) { // Start WiFi in client mode
        if (!initWIFIClient()) while (true) blinkCode(RADIO_ERROR_CODE); // Block further code execution
    }

    if (configData.wifiEnable && configData.ftpEnable) { // Start FTP server
        if (!initFTPServer()) while (true) blinkCode(RADIO_ERROR_CODE);
    }
    #endif

    // Attach the log enable button interrupt
    diagLogger->info("Attaching log enable interrupt...");
    attachInterrupt(LOG_EN, logButtonISR, FALLING);
    diagLogger->info("done!");

    TimerEvents.add(GPS_POLL_INTERVAL, pollGPS);
    TimerEvents.add(GPS_SYNC_INTERVAL*60000, syncInternalClockGPS);
    TimerEvents.add(20, []() { 
        unsigned long _fusionStartTime = micros();
        pollDSO32();
        pollLIS3MDL();
        #if defined(REV_F5) || defined(REV_G2)
        updateVoltage();
        #endif // defined(REV_F5) || defined(REV_G2)
        diagLogger->trace("Time to process sensor fusion: %d ms", millis() - _fusionStartTime);
    } );
    TimerEvents.add(logInterval, []() { 
        unsigned long _logStartTime = micros();
        dataLogger.writeTelemetryData();
        diagLogger->trace("Time to log data: %d us", micros() - _logStartTime);
    });

    setSystemState(STANDBY);
}

void loop() {
    TimerEvents.tasks();
    api.checkForCommand();

    // WiFi handling
    #ifdef WIFI_ENABLE
    if (configData.wifiEnable && configData.ftpEnable) { // Only run the FTP server when the proper configs are set and the device is not logging (efficiency)
        ftpServer.handleFTP();
    }
    #endif
    
    // State and LED updates
    updateSystemState();
    updateSystemLED();

    // Log Enabling handler
    static uint8_t _oldButtonPresses = 0;
    if (logButtonPresses != _oldButtonPresses && !digitalRead(LOG_EN) && millis() >= logButtonStartTime+LOG_BTN_HOLD_TIME) { // Check if BTN0 has been pressed and has been held for sufficient time
        isLogging = !isLogging;
        if (isLogging) {
            dataLogger.start(SD);
            digitalWrite(SD_CS, LOW);
        }
        else {
            dataLogger.stop();
        }
        digitalWrite(LED_BUILTIN, isLogging);
        _oldButtonPresses = logButtonPresses;
    }

    updateRTCms();
}

void updateSettings() {
    diagLogger->info("Updating settings...");
    // -----Sensor Configurations-----
    dso32.setAccelRange(getAccelRange(configData.accelRange));
    dso32.setGyroRange(getGyroRange(configData.gyroRange));
    dso32.setAccelDataRate(getDataRate(configData.imuDataRate));
    dso32.setGyroDataRate(getDataRate(configData.imuDataRate));
    // TODO: Add magnetometer (LIS3MDL)

    // -----Logging Configurations-----
    // timerAlarmWrite(timer, 1E6/configData.loggingUpdateRate, true);
    logFrequency = configData.loggingUpdateRate;
    logInterval = 1E6/logFrequency; // us
    // diagLogger->verbose("Set log interval to: %f seconds")
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

void IRAM_ATTR onTimer() {
    // Give a semaphore that we can check in the loop
    xSemaphoreGiveFromISR(timerSemaphore, NULL);
}