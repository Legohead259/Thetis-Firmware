/**
 * @file Thetis Firmware
 * @version 0.3.0
 * 
 * @brief This firmware variant enables Thetis to act as an embedded accelerometer.
 * Accelerometer values will be logged to the onboard storage device and can be accessible via the WiFi interface, if enabled.
 * This firmware will work on a barebones Thetis that does not have a GPS module.
 * Currently compatible with the following boards:
 *      - RevF4 with LSM6DSO32 IMU
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
 *       July 15, 2022 (last edit)
**/
const char FW_VERSION[] = "0.3.0";
const char HW_REVISION[] = "Rev F4";

// DEBUG FLAGS
// #define IMU_DEBUG
// #define GPS_DEBUG
// #define SDCARD_DEBUG
// #define SDCONFIG_DEBUG
// #define WIFI_ENABLE
#ifdef WIFI_ENABLE
    #include "WiFi.h"
    #include "ESPAsyncWebServer.h"
    char ssid[32];
    AsyncWebServer server(80); // Create AsyncWebServer object on port 80
#endif // WIFI_ENABLE

#include <ThetisLib.h>

#include <SPIFFS.h>

#define GPS_SYNC_INTERVAL 1 // minutes
#define GPS_TIMEOUT 5000 // ms
#define IMU_POLL_RATE 52.0 // Hz
#define IMU_POLL_INTERVAL 1000/IMU_POLL_RATE // ms

telemetry_t data;
char filename[13];
char timestamp[40];
Status_t currentState;

// Configuration Data
Config cfg;
bool isGPSEnable = true;
uint8_t deviceID;

// Flags
bool isDebugging = false;
bool isIMUAvailable = false;
bool isLogging = false;
bool isLogFileCreated = false;
bool isIMUCalibrated = true;

// Prototypes
void syncInternalClockGPS();
void loadConfig();
String processor(const String &var);
void updateSystemState();
void updateSystemLED();
bool writeTelemetryData();

void setup() {
    isDebugging = digitalRead(USB_DETECT); // Check if USB is plugged in
    if (isDebugging) {
        Serial.begin(115200);
        while(!Serial); // Wait for serial connection
    }

    Serial.println("-------------------------------------");
    Serial.println("    Thetis Firmware Version 0.4.0    ");
    Serial.println("-------------------------------------");
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

    Serial.print("Initializing SPIFFS...");
    if (!SPIFFS.begin()) { // Initialize internal filesystem and check if good
		Serial.print("Failed to initialize SPIFFS!");
		while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block code execution
	}
    Serial.println("done!");

    Serial.print("Loading configurations...");
    if (cfg.begin(SPIFFS, "/config.cfg", 127)) {
        Serial.println();
        while (cfg.readNextSetting()) {
            if (cfg.nameIs("id")) {
                deviceID = cfg.getIntValue();
                Serial.print("The ID of this device is configured to: ");
                Serial.println(deviceID);
            }
            else if (cfg.nameIs("gps_enable")) {
                isGPSEnable = cfg.getBooleanValue();
                Serial.print("GPS functionality has been: ");
                Serial.println(isGPSEnable ? "Enabled" : "Disabled");
            }
            else {
                Serial.print("Unknown setting name: ");
                Serial.println(cfg.getName());
            }
        }
        cfg.end();
    }
    else {
        Serial.print("Failed to open configuration file!");
        while (true) blinkCode(FILE_ERROR_CODE); // Block code execution
    }
   
    if (isGPSEnable) {
        syncInternalClockGPS();
    }

    // Attach the log enable button interrupt
    attachInterrupt(LOG_EN, logButtonISR, FALLING);

    #ifdef WIFIAP_ENABLE
        Serial.print("Starting WiFi access point...");
        sprintf(ssid, "Thetis-%03u", deviceID); // Format AP SSID based on Device ID
        if (!WiFi.softAP(ssid, "")) {
            Serial.println("Failed to start access point!");
            while (true) blinkCode(RADIO_ERROR_CODE); // Block code execution
        }
        Serial.println("done!");

        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);

        // Route for root / web page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/index.html", String(), false, processor);
        });
        
        // Route to load style.css file
        server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/style.css", "text/css");
        });

        // Start server
        server.begin();
    #endif // WIFI_ENABLE
}

void loop() {
    // State and LED updates
    updateSystemState();
    updateSystemLED();

    static long _lastGPSSync = millis();
    if (isGPSEnable && millis() >= _lastGPSSync+GPS_SYNC_INTERVAL*60000) { // Check GPS enabled and if GPS_SYNC_INTERVAL time has passed
        syncInternalClockGPS();
        _lastGPSSync = millis(); // Reset GPS sync timer flag
    }

    static long _lastIMUPoll = millis();
    if (millis() >= _lastIMUPoll+IMU_POLL_INTERVAL) { // Check if IMU_POLL_INTERVAL time has passed
        pollDSO32();

        // Write data to log file
        if (isLogging) {
            char _writeBuf[128];
            getISO8601Time_RTC(timestamp);
            sprintf(_writeBuf, "%s,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n",  
                    timestamp,
                    accel.acceleration.x, accel.acceleration.y, accel.acceleration.z,
                    linAccel.x, linAccel.y, linAccel.z);
            if (!appendFile(SD, filename, _writeBuf)) { // Check if file append was successful; enter error state if not
                while(true) blinkCode(FILE_ERROR_CODE);
            }
        }

        _lastIMUPoll = millis(); // Reset IMU poll timer
    }

    static uint8_t _oldButtonPresses = 0;
    if (logButtonPresses != _oldButtonPresses && !digitalRead(LOG_EN) && millis() >= logButtonStartTime+LOG_BTN_HOLD_TIME) { // Check if BTN0 has been pressed and has been held for sufficient time
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


// =========================
// === UTILITY FUNCTIONS ===
// =========================


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
        timeElements.Year = nmea.getYear()-1970;
        timeElements.Month = nmea.getMonth();
        timeElements.Day = nmea.getDay();
        timeElements.Hour = nmea.getHour();
        timeElements.Minute = nmea.getMinute();
        timeElements.Second = nmea.getSecond();

        setTime(makeTime(timeElements)); // Reset internal clock
        Serial.println("Done!");
    }
    else {
        Serial.println("GPS fix was not valid - did not sync");
    }
    Serial.println();
}

void updateSystemState() {
    if (!isIMUCalibrated && !data.GPSFix)                       currentState = STANDBY;
    else if (isIMUCalibrated && !data.GPSFix && !isLogging)     currentState = READY_NO_GPS;
    else if (isIMUCalibrated && data.GPSFix && !isLogging)      currentState = READY_GPS;
    else if (isIMUCalibrated && !data.GPSFix && isLogging)      currentState = LOGGING_NO_GPS;
    else if (isIMUCalibrated && data.GPSFix && isLogging)       currentState = LOGGING_GPS;
    data.state = currentState;
}

void updateSystemLED() {
    switch (data.state) {
        case LOGGING_NO_GPS:
            pixel.setPixelColor(0, BLUE); pixel.show(); // Glow solid blue
            break;
        case LOGGING_GPS:
            pixel.setPixelColor(0, GREEN); pixel.show(); // Glow solid green
            break;
        case READY_NO_GPS:
            pulseLED(BLUE); // Pulse blue
            break;
        case READY_GPS:
            pulseLED(GREEN); // Pulse green
            break;
        case STANDBY:
            pixel.setPixelColor(0, 255, 191, 0); pixel.show(); // Glow solid amber
            break;
        case BOOTING:
            pulseLED(PURPLE); // Pulse purple
            break;
        default:
            pixel.setPixelColor(0, RED); pixel.show(); // Turn off LED
            break;
    }
}

bool writeTelemetryData() {
    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.printf("Writing telemetry packet to: %s", filename);
    #endif
    File _dataFile = SD.open(filename, FILE_APPEND);
    if (!_dataFile) {
        #ifdef SDCARD_DEBUG
        DEBUG_SERIAL_PORT.printf("Could not write to %s", filename);
        #endif
        return false;
    }

    char _timestamp[32];
    getISO8601Time_RTC(_timestamp);
    _dataFile.print(_timestamp);
    _dataFile.printf("%0.3f,", data.voltage);
    _dataFile.printf("%d,", data.GPSFix);
    _dataFile.printf("%d,", data.numSats);
    _dataFile.printf("%d,", data.HDOP);
    _dataFile.printf("%0.3f,", data.latitude / 1E6);
    _dataFile.printf("%0.3f,", data.longitude / 1E6);
    _dataFile.printf("%0.3f,", data.GPSSpeed / 1E3);
    _dataFile.printf("%0.3f,", data.GPSCourse / 1E3);
    _dataFile.printf("%d,", data.sysCal);
    _dataFile.printf("%d,", data.gyroCal);
    _dataFile.printf("%d,", data.accelCal);
    _dataFile.printf("%d,", data.magCal);
    _dataFile.printf("%0.3f,", data.accelX);
    _dataFile.printf("%0.3f,", data.accelY);
    _dataFile.printf("%0.3f,", data.accelZ);
    _dataFile.printf("%0.3f,", data.magX);
    _dataFile.printf("%0.3f,", data.magY);
    _dataFile.printf("%0.3f,", data.magZ);
    _dataFile.printf("%0.3f,", data.gyroX);
    _dataFile.printf("%0.3f,", data.gyroY);
    _dataFile.printf("%0.3f,", data.gyroZ);
    _dataFile.printf("%0.3f,", data.roll);
    _dataFile.printf("%0.3f,", data.pitch);
    _dataFile.printf("%0.3f,", data.yaw);
    _dataFile.printf("%0.3f,", data.linAccelX);
    _dataFile.printf("%0.3f,", data.linAccelY);
    _dataFile.printf("%0.3f,", data.linAccelZ);
    _dataFile.printf("%0.3f,", data.quatW);
    _dataFile.printf("%0.3f,", data.quatX);
    _dataFile.printf("%0.3f,", data.quatY);
    _dataFile.printf("%0.3f,", data.quatZ);
    _dataFile.printf("%0.3f,", data.imuTemp);
    _dataFile.printf("%d,", data.state);
    _dataFile.print(data.packetSize);
    _dataFile.println();
    _dataFile.close();

    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.printf("Wrote to: %s\n\r", filename);
    #endif
    return true;
}


// ======================
// === WIFI FUNCTIONS ===
// ======================

String processor(const String &var) {
    Serial.print(var); Serial.print(": ");
    if (var == "DEVICE_ID") {
        char _deviceIDStr[4];
        sprintf(_deviceIDStr, "%03u", deviceID);
        Serial.println(_deviceIDStr);
        return _deviceIDStr;
    }
    else if (var == "FW_VERSION") {
        Serial.println(FW_VERSION);
        return FW_VERSION;
    }
    else if (var == "HW_REVISION") {
        Serial.println(HW_REVISION);
        return HW_REVISION;
    }
    Serial.println();
    return var;
}