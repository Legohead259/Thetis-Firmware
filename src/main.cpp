/**
 * @file main.cpp
 * @version 1.0.0
 * @author Braidan Duffy (bduffy2018@my.fit.edu)
 * @brief 
 * @date May 15, 2023
 *       May 15, 2023 (last edit)
 * 
 * CHANGELOG:
 * Version 1.0.0 - Initial baseline release
**/
#define __FIRMWARE_VERSION__ "1.0.0"
#include <ThetisLib.h>

#define SAMPLE_RATE (100) // Hz

unsigned long logFrequency; // Hz 
unsigned long logInterval;  // Time between log updates [ms]
unsigned long logButtonPresses;
unsigned long logButtonStartTime;

// Flags
bool isDebugging = false;

void IRAM_ATTR logButtonISR();

void setup() {
    // Casting to int is important as just uint8_t types will invoke the "slave" begin, not the master
    Wire.begin((int) SDA, (int) SCL);

    isDebugging = digitalRead(USB_DETECT); // Check if USB is plugged in
    if (isDebugging && diagPrintLogger.begin(&Serial, LogLevel::FATAL)) {
        Serial.begin(115200);
        while(!Serial); // Wait for serial connection
    }
    
    diagLogger = isDebugging ? &diagPrintLogger : &diagFileLogger;

    Serial.println("---------------------------------");
    Serial.println("    Thetis AHRS Tester v1.0.0    ");
    Serial.println("---------------------------------");
    Serial.println();

    if (!initDSO32()) { // Check IMU initialization
        Serial.println("Failed DSO32");
        while(true); // Block further code execution
    }

    if (!initLIS3MDL()) { // Check magnetometer initialization
        Serial.println("Failed LIS3MDL");
        while(true);
    }

    if (!digitalRead(SD_CARD_DETECT) || !dataLogger.begin(SD, SD_CS)) { // Initialize SD card filesystem and check if good
        diagLogger->warn("uSD Card not detected or not working, attempting backup with XTSD card");
        blinkCode((ErrorCode_t) B1010, AMBER); // Warn that there was an issue with the SD card
        if (!dataLogger.begin(SD, XTSD_CS)) { // Try to initialize backup writing to the XTSD card
            while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
        }
    }

    // Attach the log enable button interrupt
    diagLogger->info("Attaching log enable interrupt...");
    attachInterrupt(LOG_EN, logButtonISR, FALLING);
    diagLogger->info("done!");
}

void loop() {
    pollDSO32();
    pollLIS3MDL();
    
    // Update the log with the most recent sample
    static unsigned long _lastLogTime = micros();
    if (isLogging && (micros() - _lastLogTime) >= logInterval) { // Check if the log interval has passed
        unsigned long _logStartTime = micros();
        char buffer[64];
        #ifdef MAG_CAL
        sprintf(buffer, "%0.3f\t%0.3f\t%0.3f\n", data.magX, data.magY, data.magZ);
        #endif
        #ifdef ACCEL_CAL
        sprintf(buffer, "%0.3f\t%0.3f\t%0.3f\n", data.accelX, data.accelY, data.accelZ);
        #endif
        dataLogger.print(buffer);
        diagLogger->trace("Time to log data: %d us", micros() - _logStartTime);
        _lastLogTime = micros();
    }

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
    delay(50); // Delay 50 ms (20 Hz report rate)
}

void IRAM_ATTR logButtonISR() {
    logButtonPresses++;
    logButtonStartTime = millis();
}