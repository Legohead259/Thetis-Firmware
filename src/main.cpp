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

// Define calibration (replace with actual calibration data if available)
const FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
const FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
const FusionVector gyroscopeOffset = {-0.197f, -0.2145f, -0.7615f};
const FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
const FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
const FusionVector accelerometerOffset = {-0.107126f, -0.169197f, 0.203518f};
const FusionMatrix softIronMatrix = {1.007953f, 0.028542f, 0.021886f, 0.028542f, 0.997232f, -0.023602f, 0.021886f, -0.023602f, 0.983273f};
const FusionVector hardIronOffset = {-20.178613f, 184.923591f, 281.544349f};

FusionOffset offset;
FusionAhrs ahrs;

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

    // Initialise algorithms
    FusionOffsetInitialise(&offset, SAMPLE_RATE);
    FusionAhrsInitialise(&ahrs);

    // Set AHRS algorithm settings
    const FusionAhrsSettings settings = {
            .convention = FusionConventionNwu,
            .gain = 0.5f,
            .accelerationRejection = 10.0f,
            .magneticRejection = 20.0f,
            .rejectionTimeout = 5 * SAMPLE_RATE, /* 5 seconds */
    };
    FusionAhrsSetSettings(&ahrs, &settings);
}

void loop() {
    // Acquire latest sensor data
    unsigned long timestamp = micros(); // replace this with actual gyroscope timestamp
    pollDSO32();
    pollLIS3MDL();
    FusionVector gyroscope = {data.gyroX, data.gyroY, data.gyroZ};
    FusionVector accelerometer = {data.accelX, data.accelY, data.accelZ};
    FusionVector magnetometer = {data.magX, data.magY, data.magZ};

    // Apply calibration
    gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
    accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
    magnetometer = FusionCalibrationMagnetic(magnetometer, softIronMatrix, hardIronOffset);

    // Update gyroscope offset correction algorithm
    gyroscope = FusionOffsetUpdate(&offset, gyroscope);

    // Calculate delta time (in seconds) to account for gyroscope sample clock error
    static unsigned long previousTimestamp;
    const float deltaTime = (float) (timestamp - previousTimestamp) / (float) CLOCKS_PER_SEC;
    previousTimestamp = timestamp;

    // Update gyroscope AHRS algorithm
    FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);

    // Print algorithm outputs
    const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
    const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);

    // printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f, X %0.1f, Y %0.1f, Z %0.1f\n",
    //         euler.angle.roll, euler.angle.pitch, euler.angle.yaw,
    //         earth.axis.x, earth.axis.y, earth.axis.z);
    Serial.printf("Orientation: %0.3f, %0.3f, %0.3f\n", euler.angle.yaw, euler.angle.pitch, euler.angle.roll);

    // Serial.printf("Orientation: %f, %f, %f\n", 360-getYaw(), getPitch(), getRoll());
    // Serial.printf("Quaternion: %f, %f, %f, %f\n", data.quatW, data.quatX, data.quatY, data.quatZ);
    delay(50); // Delay 50 ms (20 Hz report rate)
}