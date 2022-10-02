#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define EXTERNAL_NUM_INTERRUPTS 46
#define NUM_DIGITAL_PINS        48
#define NUM_ANALOG_INPUTS       20

#define analogInputToDigitalPin(p)  (((p)<20)?(analogChannelToDigitalPin(p)):-1)
#define digitalPinToInterrupt(p)    (((p)<48)?(p):-1)
#define digitalPinHasPWM(p)         (p < 46)

#define GPS_SERIAL_PORT Serial1
#define DEBUG_SERIAL_PORT Serial

// UART
static const uint8_t TX0 = 43;
static const uint8_t RX0 = 44;
static const uint8_t TX1 = 17;
static const uint8_t RX1 = 18;

// I2C
static const uint8_t SDA = 33;
static const uint8_t SCL = 34;

// SPI Bus
static const uint8_t SS    = 26;
static const uint8_t SD_CS = 26;
static const uint8_t MOSI  = 35;
static const uint8_t MISO  = 37;
static const uint8_t SCK   = 36;

// Analog Inputs
static const uint8_t A0 = 1;
static const uint8_t A1 = 2;
static const uint8_t A2 = 3;
static const uint8_t A3 = 4;
static const uint8_t A4 = 5;
static const uint8_t A5 = 6;
static const uint8_t A6 = 7;
static const uint8_t A7 = 8;
static const uint8_t A8 = 9;
static const uint8_t A9 = 10;
static const uint8_t A10 = 11;
static const uint8_t A11 = 12;
static const uint8_t A12 = 13;
static const uint8_t A13 = 14;
static const uint8_t A14 = 15;
static const uint8_t A15 = 16;
static const uint8_t A16 = 17;
static const uint8_t A17 = 18;
static const uint8_t A18 = 19;
static const uint8_t A19 = 20;

// RTC?
static const uint8_t T1 = 1;
static const uint8_t T2 = 2;
static const uint8_t T3 = 3;
static const uint8_t T4 = 4;
static const uint8_t T5 = 5;
static const uint8_t T6 = 6;
static const uint8_t T7 = 7;
static const uint8_t T8 = 8;
static const uint8_t T9 = 9;
static const uint8_t T10 = 10;
static const uint8_t T11 = 11;
static const uint8_t T12 = 12;
static const uint8_t T13 = 13;
static const uint8_t T14 = 14;

// DAC
static const uint8_t DAC1 = 17;
static const uint8_t DAC2 = 18;

// Thetis-Specific Pins
static const uint8_t BOOT               = 0; // Input
static const uint8_t BNO055_RESET       = 7; // Output, resets the IMU
static const uint8_t USB_DETECT         = 12; // Input, detects USB presence
static const uint8_t BATTERY_MONITOR    = 15; // Input, detects battery voltage
static const uint8_t SD_CARD_DETECT     = 16; // Input, detects SD card presence
static const uint8_t LED_BUILTIN        = 38; // Output, onboard activity LED
static const uint8_t NEOPIXEL_EN        = 39; // Output, enable the Neopixel power; Active LOW
static const uint8_t NEOPIXEL_DATA      = 40; // Output, data for the Neopixel
static const uint8_t LOG_EN             = 41; // Input, enables/disables logging; Active LOW
static const uint8_t GPS_PPS            = 42; // Input, a hardware-timed signal from the GPS for synchronicity

#endif /* Pins_Arduino_h */
