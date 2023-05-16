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

// Flags
bool isDebugging = false;

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
}

void loop() {
    pollDSO32();
    pollLIS3MDL();
    
    Serial.printf("Uni:%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n", data.accelX, data.accelY, data.accelZ,
                                                                                data.gyroX, data.gyroY, data.gyroZ,
                                                                                data.magX, data.magY, data.magZ);
    delay(50); // Delay 50 ms (20 Hz report rate)
}