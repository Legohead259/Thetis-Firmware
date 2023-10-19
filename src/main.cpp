/**
 * @file main.cpp
 * @version 1.3.0
 * @author Braidan Duffy (bduffy2018@my.fit.edu)
 * @brief 
 * @date June 10, 2022
 *       June 16, 2023 (last edit)
 * 
 * CHANGELOG:
 * Version 0.1.0 - Initial baseline release
 * Version 0.2.0 - Added GPS integration
 * Version 0.2.1 - Integrated internal RTC for timestamps
 * Version 0.2.2 - Fixed log file write issue and implemented log enable functionality
 * Version 0.2.3 - Fixed major bug with the device crashing on log enabled
 * Version 0.3.0 - Added loading configurations from file on SPIFFS; added WiFi hotspot functionality
 * Version 1.0.0 - Complete codebase refactor to make development easier and streamline multiple things
 * Version 1.1.0 - Optimizing logging process for increased sample rate; added more configuration options
 * Version 1.1.1 - Fixed issue where internal RTC was not syncing to GPS time on startup
 * Version 1.1.2 - Enabled faster logging rates up to ~90 Hz
 * Version 1.1.3 - Massive overhaul to logging system
 * Version 1.2.0 - Added FTP server - began implementing WiFi functionality
 * Version 1.2.1 - Added asynchronous server for enable/disabling logging over HTTP GET requests
 * Version 1.2.2 - Fixed NeoPixel functionality
 * Version 1.2.3 - Added magnetometer functionality
 * Version 1.2.4 - Added battery gauge
 *               - Changed the way logging interval is handled; it is now user configurable
 *               - Reintroduced RTC timestamp bug fix
 * Version 1.2.5 - Fixed GPS polling issue
 *               - Reverted log polling method
 * Version 1.3.0 - Integrated TimerEvents rework
 *               - Integrated xioAPI (ThetisAPI)
 *               - Integrated Fusion rework
 * Version 2.0.0 - Migrated all features to a dedicated OOP-compliant Thetis object
**/
#define __FIRMWARE_VERSION__ "2.0.0"

#include <Thetis.h>

void setup() {
    board.initialize();
}

void loop() {
    board.run();
}