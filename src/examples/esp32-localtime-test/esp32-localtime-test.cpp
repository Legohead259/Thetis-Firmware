#include <Arduino.h>
#include <TimeLib.h>
#include <MicroNMEA.h>
#include <ThetisLib.h>

#define GPS_SYNC_INTERVAL 1 // Minutes
#define GPS_SYNC_TIMEOUT 5000 // ms

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;

bool getTime(const char *str);
bool getDate(const char *str);
long getmSeconds();
void syncInternalClockGPS();

void setup() {
    Serial.begin(115200);
    while(!Serial); // Wait for Serial port to open

    Serial.println("-----------------------------");
    Serial.println("   ESP32 Local Time Tester   ");
    Serial.println("-----------------------------");
    Serial.println();
	
    // Set current date time from compile information
	getTime(__TIME__);
	getDate(__DATE__);
	setTime(makeTime(tm));

    if (!initGPS()) { // Initialize GPS, halt code if failed
        while(true); // Halt code execution
    }
    // MicroNMEA::sendSentence(GPS, "$PMTK220,2000"); // Set GPS update rate to 2000 ms
    
    delay(100);
    syncInternalClockGPS();
}

void loop() {
    static long _lastSyncCheck = millis(); 
    if (millis() >= _lastSyncCheck + GPS_SYNC_INTERVAL*60000) { // Check if GPS_SYNC_INTERVAL minutes have passed since last sync
        syncInternalClockGPS();
        _lastSyncCheck = millis();
    }

    breakTime(now(), tm);
    static long _lastSecond = 0;
    static long _lastMSecond = 0;
    long curMSecond = millis();
    if (tm.Second == _lastSecond) {
        curMSecond = millis() - _lastMSecond;
        // Serial.println((int) curMSecond); //DEBUG
    }
    else {
        _lastSecond = tm.Second;
        _lastMSecond = millis();
        curMSecond = 0;
    }

    static long _lastPrintCheck = millis();
    if (millis() >= _lastPrintCheck+900) { // Print the time every second 
        Serial.printf("%04d-%02d-%02dT%02d:%02d:%02d.%03d\n\r", tm.Year+1970, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second, curMSecond);
        _lastPrintCheck = millis();
    }
}

void syncInternalClockGPS() {
    Serial.println();
    Serial.print("Attempting to sync internal clock to GPS time...");
    long timeoutStart = millis();
    while(!GPS) { // Wait for a GPS message to arrive
        if (millis() >= timeoutStart+GPS_SYNC_TIMEOUT) return; // Stop attempt, if TIMEOUT occurs
    }

    while(GPS.available()) { // Check for an available GPS message
        char c = GPS.read();
        Serial.print(c); // Debug
        nmea.process(c);
    }
    if (nmea.isValid()) { // If the GPS has a good fix, reset the internal clock to the GPS time
        tm.Year = nmea.getYear()-1970;
        tm.Month = nmea.getMonth();
        tm.Day = nmea.getDay();
        tm.Hour = nmea.getHour();
        tm.Minute = nmea.getMinute();
        tm.Second = nmea.getSecond();

        setTime(makeTime(tm)); // Reset internal clock
        Serial.println("Done!");
    }
    else {
        Serial.println("GPS fix was not valid - did not sync");
    }
    Serial.println();
}

bool getTime(const char *str) {
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str) {
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}