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

## 0.2.3 - 2022-07-14

### Fixed
- Fix device crashing after enabling log while parsing timestamps ([Thetis-Firmware #1](https://github.com/Legohead259/Project-Thetis-Firmware/issues/1))

---

## 0.2.2 - 2022-07-13 [YANKED]
### Known Bugs
- [CRITICAL] Device crashes after enabling log while parsing timestamps ([Thetis-Firmware #1](https://github.com/Legohead259/Project-Thetis-Firmware/issues/1))
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
