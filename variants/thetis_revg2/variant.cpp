/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Ha Thach (tinyusb.org) for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "esp32-hal-gpio.h"
#include "pins_arduino.h"

extern "C" {

// Initialize variant/board, called before setup()
void initVariant(void) {
    // Initialize Input/Output pins
    pinMode(BOOT, INPUT);
    pinMode(NEOPIXEL_EN, OUTPUT);
    pinMode(LOG_EN, INPUT);
    pinMode(SD_CARD_DETECT, INPUT);
    pinMode(GPS_RESET, OUTPUT);
    pinMode(BATT_MONITOR, INPUT);
    pinMode(USB_DETECT, INPUT);
    pinMode(CAN_RESET, OUTPUT);
    pinMode(CAN_INT, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(NEOPIXEL_DATA, OUTPUT);
    
    // Start I2C bus
    // Wire.begin(SDA, SCL);
}
}
