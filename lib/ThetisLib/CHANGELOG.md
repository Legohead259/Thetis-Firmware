# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

<!-- 
Release sections
### Known Bugs
### Added
### Changed
### Fixed
### Deprecated
### Removed
### Security 
-->

## Unreleased

### **TODO**

---

## 1.1.5 - 2022-07-22
### Fixed
- Compile error with `currentState`; moved declaration to main application
- Compile error with `writeTelemetryData()`; moved function to main application

## 1.1.4 - 2022-07-22 [REVOKED]
### Added
- Added `GPS_DEBUG` flag support for debug print statements with the GPS radio ([#6](https://github.com/Legohead259/ThetisLib/issues/6))
- Added 'BNO055_DEBUG` flag support for debug print statements with the BNO055 sensor ([#6](https://github.com/Legohead259/ThetisLib/issues/6))
- Added 'LSM6DSO_DEBUG` flag support for debug print statements with the LSM6DSO32 sensor ([#6](https://github.com/Legohead259/ThetisLib/issues/6))

### Changed
- Moved `writeTelemetryData()` to `ThetisFS` instead of main library file

## 1.1.3 - 2022-07-22
### Added
- Added `NEOPIXEL_DEBUG` flag support for debug print statements with the NeoPixel features ([#6](https://github.com/Legohead259/ThetisLib/issues/6)) 
### Changed
- Changed LED pulse behavior from a linear cylce to sinusoidal

### Fixed
- Fixed LED pulse behavior to be more noticable ([#7](https://github.com/Legohead259/ThetisLib/issues/7))
---

## 1.1.2 - 2022-07-20
### Fixed
- Fixed debug serial compile error by declaring it at the top of `ThetisFS.h`. NOTE: This needs to be done in the Thetis variants file
- Fixed compile error due to old `tm` variable calls in `ThetisLib.h`; replaced with `timeElements`.
---

## 1.1.1 - 2022-07-20
### Fixed
- Compile error due to a random 'S' in `ThetisLib.h`
---

## 1.1.0 - 2022-07-20
### Changed
- Moved status enumerator to main library file from NeoPixel library
- Changed `telemetry_t.state` data type to `Status_t` from `uint8_t`
---

## 1.0.0 - 2022-07-17
### Added
- Issue templates
- Added License
- Information required for release
---

## 0.3.0 - 2022-07-15
### Added
- Added config file library and functions

### Fixed
- Renamed time elements variable, `tm` to maintain compatibility with WiFi libraries

### Removed
- Dependancy on Ethernet library
---

## 0.2.2 - 2022-07-14
### Fixed
- Buffer overflow error when formatting timestamp ([Thetis-Firmware #1](https://github.com/Legohead259/Thetis-Firmware/issues/1))

## 0.2.1 - 2022-07-01
### Known Bugs
- [CRITICAL] Thetis crashes on log enable ([Thetis-Firmware #1](https://github.com/Legohead259/Thetis-Firmware/issues/1))

### Added
- Broke out NeoPixel functions to seperate files
- Added ability to set headers when calling `initLogFile()`
- Added function to parse system time to ISO8601 standard

### Changed
- Changed library version number in properties file

### Fixed
- Arguments issue with prototype functions in `ThetisFS.h`
- Major bug in `getISO8601Time_RTC()` prototype function definition in `ThetisLib.h`
- Various oither minor bugs
- Log enable counter not starting properly

### Removed
- Removed some redundant NeoPixel functions
---

## 0.2.0 - 2022-06-25
### Added
- Added filters and IMU helps to the IMU library
- 
### Changed
- Change Mahoney AHRS to accept different kp and ki and refresh rate parameters on construction
- Began fragmenting library into seperate header files in corresponding folders
---

## 0.1.3 - 2022-06-17
### Added
- Added 2nd Order Butterworth filter
---

## 0.1.2 - 2022-06-10
### Added
- Added telemetry file handlers
- Added the telemetry data packet definition
---

## 0.1.1 - 2022-06-03
### Added
- Added GPS callback functions and global functions

### Fixed
- Fix a bug with the reference pointer in `initLSM6DSO32()`

### Removed
- Removed DEBUG statements in the log button ISR
---

## 0.1.0 - 2022-06-01
### Added 
- Core functionality
- Added initialization functions for BNO055 and LSM6DSO
- Added GPS initialization function
- Added NeoPixel functions
---
