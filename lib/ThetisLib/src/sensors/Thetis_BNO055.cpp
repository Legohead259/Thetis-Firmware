#include "Thetis_BNO055.h"

Adafruit_BNO055 bno;

bool initBNO055() {
    #ifdef BNO055_DEBUG
    DEBUG_SERIAL_PORT.print("Initializing BNO055...");
    #endif
    digitalWrite(BNO055_RESET, HIGH); // Ensure BNO Reset is HIGH
    if (!bno.begin()) {
        #ifdef BNO055_DEBUG
        DEBUG_SERIAL_PORT.println("Failed to initialize BNO055");
        #endif
        return false;
    }
    else {
        bno.setExtCrystalUse(true);
        #ifdef BNO055_DEBUG
        DEBUG_SERIAL_PORT.println("done!");
        #endif
        return true;
    }
}