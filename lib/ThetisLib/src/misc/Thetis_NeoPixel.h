#ifndef THETIS_NEOPIXEL_H
#define THETIS_NEOPIXEL_H

// ==========================
// === NEOPIXEL FUNCTIONS ===
// ==========================

// Neopixel instantiation
#include <Adafruit_NeoPixel.h>
#define DASH_ON 500 // ms
#define DOT_ON 125  // ms
#define BLINK_INTERVAL 125 // ms
#define MESSAGE_INTERVAL 1000 // ms
#define MAXIMUM_BRIGHTNESS 32
#define NUM_STEPS 50
#define BRIGHTNESS_STEP MAXIMUM_BRIGHTNESS/NUM_STEPS
#define NEOPIXEL_CYCLE_TIME 3000 // ms - Bright to bright time

/*
Code Table:
Error       |  DOT  |  DASH  |  DOT  |  DASH  |  CODE  |  COLOR  |  DESCRIPTION
------------|-------|--------|-------|--------|--------|---------|--------------------------------------------------
General     |   0   |   0    |   0   |    1   |  B0001 |   RED   | Unknown, but critical failure
Card Mount  |   0   |   0    |   1   |    0   |  B0010 |   RED   | Filesystem fails to mount
Card Type   |   0   |   0    |   1   |    1   |  B0011 |   RED   | Filesystem initializes, but reports a bad type
File Open   |   0   |   1    |   0   |    0   |  B0100 |   RED   | Datalog file fails to open
Radio       |   0   |   1    |   0   |    1   |  B0101 |   RED   | Radio fails to initialize/encounters error
GPS         |   0   |   1    |   1   |    0   |  B0110 |   RED   | GPS radio fails to initialize
IMU         |   0   |   1    |   1   |    1   |  B0111 |   RED   | IMU fails to initialize
Low Battery |   1   |   0    |   0   |    0   |  B1000 |  AMBER  | Battery voltage is below 3.0V

A dot is DOT_ON on, DOT_ON off
A dash is DASH_ON on, DASH_ON off
Space between codes is MESSAGE_INTERVAL
*/

typedef enum ErrorCode {
    GEN_ERROR_CODE          = B0001,
    CARD_MOUNT_ERROR_CODE   = B0010,
    CARD_TYPE_ERROR_CODE    = B0011,
    FILE_ERROR_CODE         = B0100,
    RADIO_ERROR_CODE        = B0101,
    GPS_ERROR_CODE          = B0110,
    IMU_ERROR_CODE          = B0111,
    LOW_BATT_ERROR_CODE     = B1000
} ErrorCode_t;
 
extern Adafruit_NeoPixel pixel;

const uint32_t OFF      = Adafruit_NeoPixel::Color(0, 0, 0);       // GRB
const uint32_t WHITE    = Adafruit_NeoPixel::Color(255, 255, 255);
const uint32_t BLUE     = Adafruit_NeoPixel::Color(0, 0, 255);
const uint32_t RED      = Adafruit_NeoPixel::Color(0, 255, 0);
const uint32_t GREEN    = Adafruit_NeoPixel::Color(255, 0, 0);
const uint32_t PURPLE   = Adafruit_NeoPixel::Color(0, 255, 255);
const uint32_t AMBER    = Adafruit_NeoPixel::Color(191, 255, 0);
const uint32_t CYAN     = Adafruit_NeoPixel::Color(255, 0, 255);
const uint32_t LIME     = Adafruit_NeoPixel::Color(125, 0, 255);

const float brightness = 0.1;

void initNeoPixel();
void pulseLED(uint32_t color);
void rainbow();
uint32_t Wheel(byte wheelPos);
void blinkCode(ErrorCode_t code, uint32_t color=RED);

#endif // ndef THETIS_NEOPIXEL_H