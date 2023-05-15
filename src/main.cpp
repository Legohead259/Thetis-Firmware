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

Madgwick filter;
unsigned long microsPerReading, microsPrevious;
float accelScale, gyroScale;

// Flags
bool isDebugging = false;
bool isIMUAvailable = false;
bool isLogFileCreated = false;
bool isIMUCalibrated = true;

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
        while(true); // Block further code execution
    }

    if (!initLIS3MDL()) { // Check magnetometer initialization
        while(true);
    }

    microsPerReading = 1000000 / 25;
    microsPrevious = micros();
}

void loop() {
    int aix, aiy, aiz;
    int gix, giy, giz;
    float ax, ay, az;
    float gx, gy, gz;
    float roll, pitch, heading;

    // check if it's time to read data and update the filter
    if (micros() - microsPrevious >= microsPerReading) {

        // read raw data from MARG
        pollDSO32();
        pollLIS3MDL();

        // update the filter, which computes orientation
        filter.update(data.gyroX, data.gyroY, data.gyroZ, 
                        data.accelX, data.accelY, data.accelZ, 
                        data.magX, data.magY, data.magZ);

        // print the heading, pitch and roll
        Serial.printf("Orientation: %f, %f, %f\n", 360-filter.getYaw(), filter.getPitch(), filter.getRoll());
        // Serial.printf("Quaternion: %f, %f, %f, %f\n", filter.w, filter.x, filter.y, filter.z);

        // increment previous time, so we keep proper pace
        microsPrevious = microsPrevious + microsPerReading;
    }

    // Serial.printf("Orientation: %f, %f, %f\n", 360-getYaw(), getPitch(), getRoll());
    // Serial.printf("Quaternion: %f, %f, %f, %f\n", data.quatW, data.quatX, data.quatY, data.quatZ);
    // delay(50); // Delay 50 ms (20 Hz report rate)
}