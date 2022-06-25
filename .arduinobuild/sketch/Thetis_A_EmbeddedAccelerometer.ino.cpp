#include <Arduino.h>
#line 1 "c:\\Users\\legoh\\OneDrive - Florida Institute of Technology\\Projects\\Project Poseidon\\Project Thetis\\Firmware\\Thetis_A_EmbeddedAccelerometer\\Thetis_A_EmbeddedAccelerometer.ino"
/**
 * @file Thetis Firmware Variant A - Embedded Accelerometer
 * @version 0.1.0
 * 
 * @brief This firmware variant emables Thetis to act as an embedded accelerometer.
 * Accelerometer values will be logged to the onboard storage device and can be accesible via the WiFi interface, if enabled.
 * This firmware will work on a barebones Thetis that does not have a GPS module.
 * Currently compatible with the following boards:
 *      - RevF4 with LSM6DSO32 IMU
 * 
 * @author Braidan Duffy
 * @date June 10, 2022
**/
#include <ThetisLib.h>

telemetry_t data;
bool isIMUAvailable = false;
bool DEBUG_MODE = false;
char filename[12];

#line 21 "c:\\Users\\legoh\\OneDrive - Florida Institute of Technology\\Projects\\Project Poseidon\\Project Thetis\\Firmware\\Thetis_A_EmbeddedAccelerometer\\Thetis_A_EmbeddedAccelerometer.ino"
void setup();
#line 51 "c:\\Users\\legoh\\OneDrive - Florida Institute of Technology\\Projects\\Project Poseidon\\Project Thetis\\Firmware\\Thetis_A_EmbeddedAccelerometer\\Thetis_A_EmbeddedAccelerometer.ino"
void loop();
#line 21 "c:\\Users\\legoh\\OneDrive - Florida Institute of Technology\\Projects\\Project Poseidon\\Project Thetis\\Firmware\\Thetis_A_EmbeddedAccelerometer\\Thetis_A_EmbeddedAccelerometer.ino"
void setup() {
    DEBUG_MODE = digitalRead(USB_DETECT); // Check if USB is plugged in
    if (DEBUG_MODE) {
        Serial.begin(115200);
        while(!Serial); // Wait for serial connection
    }

    Serial.println("-------------------------------------------------------");
    Serial.println("   Thetis Firmware Variant A: Embedded Accelerometer   ");
    Serial.println("-------------------------------------------------------");
    Serial.println();

    initNeoPixel();

    isIMUAvailable = DSO32::init();
    if (!isIMUAvailable) { // Check IMU initialization
        while(true) blinkCode(IMU_ERROR_CODE); // Block further code execution
    }

    if (!initSDCard()) { // Initialize filesystem and check if good
        while(true) blinkCode(CARD_MOUNT_ERROR_CODE); // Block further code execution
    }
    
    if (!initLogFile(SD, filename)) { // Initialize log file and check if good
        while(true) blinkCode(FILE_ERROR_CODE); // block further code execution
    }
    // Write header for the log file
    writeFile(SD, filename, "Timestamp (ISO 8601), ax, ay, az, lin_ax, lin_ay, lin_az");
}

void loop() {
    // pollLSM6DSO32();
    // delay(1000/accelSampleFreq);
}
