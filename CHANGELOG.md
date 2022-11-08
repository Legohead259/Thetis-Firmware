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

## [Unreleased] 1.2.0 -
### TODO

## [Unreleased] 1.1.0 - 

### TODO
- Add ability to configure logging rate from configuration file
- Add ability to configure fusion update rate from configuration file

### Added
- Added [example script](scripts/decode_log_file.py) to decode log file data retrieved from device
- Added several debug timing printouts

### Changed
- Changed the logging format to be binary instead of ASCII
- Modified the logging procedure to separate the fusion update function from the logging function

---

## 1.0.1 - 2022-10-13

### Added
- Added a `gpsPoll()` call to the main execution loop
- Added ability to log raw IMU data using compiler flags

---

## 1.0.0 - 2022-10-06 

### Added
- Added a boards/ and variants/ folder to the workspace to hold board specific information for each development board (deprecates [Thetis Package](https://github.com/Legohead259/Thetis-Package.git))
- Added git submodule for [ThetisLib](https://github.com/Legohead259/ThetisLib.git)
- Implemented data collection and logging functionality

### Changed
- Complete code refactor to reorganize and make easier to read
- Migrated many functions over to independent files inside `ThetisLib`

### Fixed
- Fixed multiple definition error with `data`
- Changed `SD_CS` and `SS` pin definitions to fix PSRAM crash issue
- 

---

## 0.3.1 - 2022-07-22
### Added
- Added the `writeTelemetryData()` function to be local
- Added local declaration for `currentState`

---

## 0.3.0 - 2022-07-15
### Added
- Added loading configurations from config file stored in SPIFFS [#2](https://github.com/Legohead259/Project-Thetis-Firmware/issues/2)
- Added core WiFi hotspot functionality ([#3](https://github.com/Legohead259/Project-Thetis-Firmware/issues/3))
    - Can load index.html file from SPIFFS
    - Can load style.css file from SPIFFS
    - Can be enabled or disabled with the build flag `-D WIFI_ENABLE`

### Changed
- Renamed time elements variable, `tm`, to `timeElements` to enable compatibility with WiFi library
---

## 0.2.3 - 2022-07-14

### Fixed
- Fix device crashing after enabling log while parsing timestamps ([#1](https://github.com/Legohead259/Project-Thetis-Firmware/issues/1))

---

## 0.2.2 - 2022-07-13 [YANKED]
### Known Bugs
- [CRITICAL] Device crashes after enabling log while parsing timestamps ([#1](https://github.com/Legohead259/Project-Thetis-Firmware/issues/1))
### Added
- Log enable functionality by holding the LOG_EN button for 1 second

### Changed
- Changed how the logging functionality creates a new log file
    - Previously, it always created a new log file on startup, which created a lot of clutter while debugging
    - Now, the new log file is only created on the first enabling of log between microcontroller restarts
  
### Fixed
- Log file writing format to append data to a new line instead of overwrite the file
## 0.2.1 - 2022-06-24
### Added
- Parser function to breakdown the internal time from the RTC into standard timestamp format
- Ability to update internal RTC with GPS timestamp data
---

## 0.2.0 - 2022-06-17
### Added
- GPS integration for reporting Lat/Lon and time
- Added ability to log data to file on the SD card
---

## 0.1.0 - 2022-06-10
### Added 
- Core functionality
- Ability to read sensors and output data to serial terminal
---
