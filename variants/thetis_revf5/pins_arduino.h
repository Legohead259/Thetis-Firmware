#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define EXTERNAL_NUM_INTERRUPTS 46
#define NUM_DIGITAL_PINS        48
#define NUM_ANALOG_INPUTS       20

#define analogInputToDigitalPin(p)  (((p)<20)?(analogChannelToDigitalPin(p)):-1)
#define digitalPinToInterrupt(p)    (((p)<48)?(p):-1)
#define digitalPinHasPWM(p)         (p < 46)

#define GPS_SERIAL_PORT Serial0
#define DEBUG_SERIAL_PORT Serial

// UART
static const uint8_t TX0 = 43;
static const uint8_t RX0 = 44;
static const uint8_t TX1 = 17;
static const uint8_t RX1 = 16;

// I2C
static const uint8_t SDA = 33;
static const uint8_t SCL = 34;

// SPI Bus
static const uint8_t SS         = 21;
static const uint8_t SD_CS      = 21;
static const uint8_t XTSD_CS    = 14;
static const uint8_t MOSI       = 35;
static const uint8_t MISO       = 37;
static const uint8_t SCK        = 36;

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
static const uint8_t BOOT           = 0; // Input
static const uint8_t GPS_RESET      = 1; // Output, resets GPS
static const uint8_t GPS_PPS        = 2; // Input, a hardware-timed signal from the GPS for synchronicity
static const uint8_t NEOPIXEL_DATA  = 3; // Output, data for the NeoPixel
static const uint8_t LOG_EN         = 8; // Input, enables/disables logging; Active LOW
static const uint8_t NEOPIXEL_EN    = 9; // Output, enable the NeoPixel power; Active LOW
static const uint8_t USB_DETECT     = 12; // Input, detects USB presence
static const uint8_t SD_CARD_DETECT = 14; // Input, detects SD card presence
static const uint8_t BATTERY_ALERT  = 38; // Input, alarm triggered by battery monitoring IC when battery is critical; Active LOW
static const uint8_t LED_BUILTIN    = 42; // Output, onboard activity LED

#endif /* Pins_Arduino_h */
