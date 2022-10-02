#ifndef THETIS_BNO055_H
#define THETIS_BNO055_H


// ========================
// === BNO055 FUNCTIONS ===
// ========================


#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Wire.h>

extern Adafruit_BNO055 bno;

bool initBNO055();

#endif // THETIS_BNO055_H