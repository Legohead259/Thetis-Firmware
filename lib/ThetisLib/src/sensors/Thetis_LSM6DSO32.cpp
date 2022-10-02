#include "Thetis_LSM6DSO32.h"

// ===========================
// === LSM6DSO32 FUNCTIONS ===
// ===========================


Adafruit_LSM6DSO32 dso32;
Kalman kalmanX;
Kalman kalmanY;
Kalman kalmanZ;

Mahony mahony(52); // Instantiate a Mahony filter with a sample frequency of 52 Hz

double accelSampleFreq; // Accelerometer sampling frequency
double gyroSampleFreq; // Gyroscope sampling frequency

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

sensors_vec_t linAccel;
sensors_vec_t accelAngle;
sensors_vec_t ufAngle;
sensors_vec_t cfAngle;
sensors_vec_t kfAngle;
sensors_vec_t eulerAngles;

bool initDSO32( lsm6dso32_accel_range_t accelRange, 
                lsm6ds_gyro_range_t gyroRange,
                lsm6ds_data_rate_t dataRate) {
    #ifdef LSM6DSO_DEBUG
    DEBUG_SERIAL_PORT.print("Initializing DSO32 IMU...");
    #endif
    if (!dso32.begin_I2C(0x6B)) {
        #ifdef LSM6DSO_DEBUG
        DEBUG_SERIAL_PORT.println("Failed to find LSM6DSO32 chip");
        #endif
        return false;
    }
    else {
        dso32.setAccelRange(accelRange);
        dso32.setGyroRange(gyroRange);
        dso32.setAccelDataRate(dataRate);
        dso32.setGyroDataRate(dataRate);
        accelSampleFreq = setSampleFrequency(dataRate);
        gyroSampleFreq = setSampleFrequency(dataRate);
        #ifdef LSM6DSO_DEBUG
        DEBUG_SERIAL_PORT.println("done!");
        #endif
        return true;
    }
}

void pollDSO32() {
    dso32.getEvent(&accel, &gyro, &temp);

    // Update AHRS filter
    mahony.updateIMU(gyro.gyro.x, gyro.gyro.y, gyro.gyro.z,
                      accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);

    // Calculate the linear accelerations
    linAccel = calcLinAccel();

    // Convert gyro to deg/s from rad/s
    gyro.gyro.x *= RAD_TO_DEG;
    gyro.gyro.y *= RAD_TO_DEG;
    gyro.gyro.z *= RAD_TO_DEG;

    // Debug print statements
    #ifdef LSM6DSO_DEBUG
    DEBUG_SERIAL_PORT.printf("Accel X: %0.3f \tY: %0.3f \tZ: %0.3f m/s/s\n\r", accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);
    DEBUG_SERIAL_PORT.printf(" Gyro X: %0.3f \tY: %0.3f \tZ: %0.3f rad/s\n\r", gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
    DEBUG_SERIAL_PORT.printf("Temperature: %0.3f °C\n\n\r", temp.temperature);
    #endif // LSM6DSO_DEBUG

    #ifdef LSM6DSO_DEBUG_PLOTTER
    // Serial plotter print statements
    DEBUG_SERIAL_PORT.printf("%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n\r",data.accelX, data.accelY, data.accelZ, 
                                                            data.linAccelX, data.linAccelY, data.linAccelZ);
    #endif // LSM6DSO_DEBUG_PLOTTER
}

sensors_vec_t calcLinAccel() {
    sensors_vec_t linAccel;

    // Graviational acceleration in NED coordinate system
    imu::Vector<3> gravGlobal = {0, 0, 9.81};
    imu::Quaternion Qb = mahony.getQuaternion();
    imu::Quaternion gravBody = Qb.invert() * imu::Quaternion(0, gravGlobal) * Qb;
    // Serial.printf("X: %0.3f \t\t Y: %0.3f \t\t Z: %0.3f \t\t m/s/s\n\r", gravBody.x(), gravBody.y(), gravBody.z());

    linAccel.x = accel.acceleration.x - gravBody.x();
    linAccel.y = accel.acceleration.y - gravBody.y();
    linAccel.z = accel.acceleration.z - gravBody.z();

    // DEBUG statement
    #ifdef LSM6DSO_DEBUG
    DEBUG_SERIAL_PORT.printf("LinAccel X: %0.3f \t\t Y: %0.3f \t\t Z: %0.3f \t\t m/s/s\n\r", linAccel.x, linAccel.y, linAccel.z);
    #endif

    return linAccel;
}

double setSampleFrequency(lsm6ds_data_rate_t dataRate) {
    double _res;
    switch (dataRate) {
        case LSM6DS_RATE_SHUTDOWN:
            _res = 0;
            break;
        case LSM6DS_RATE_12_5_HZ:
            _res = 12.5;
            break;
        case LSM6DS_RATE_26_HZ:
            _res = 26;
            break;
        case LSM6DS_RATE_52_HZ:
            _res = 52;
            break;
        case LSM6DS_RATE_104_HZ:
            _res = 104;
            break;
        case LSM6DS_RATE_208_HZ:
            _res = 208;
            break;
        case LSM6DS_RATE_416_HZ:
            _res = 416;
            break;
        case LSM6DS_RATE_833_HZ:
            _res = 833;
            break;
        case LSM6DS_RATE_1_66K_HZ:
            _res = 1660;
            break;
        case LSM6DS_RATE_3_33K_HZ:
            _res = 3330;
            break;
        case LSM6DS_RATE_6_66K_HZ:
            _res = 6660;
            break;
    }
    return _res;
}
