/*
 * FtpServer esp8266 and esp32 with SD
 *
 * AUTHOR:  Renzo Mischianti
 *
 * https://www.mischianti.org/2020/02/08/ftp-server-on-esp8266-and-esp32
 *
 */

#include <Arduino.h>
#include <WiFi.h>
#include "SD.h"

#include <SimpleFTPServer.h>

const char* ssid = "adafruit";
const char* password = "ffffffff";

FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace);
void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize);

void setup() {
    Serial.begin(115200);
    while(!Serial); // Wait for serial port to open
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());


    /////FTP Setup, ensure SPIFFS is started before ftp;  /////////

    /////FTP Setup, ensure SPIFFS is started before ftp;  /////////

    Serial.print("Initializing SD Card...");
    if (!SD.begin()) {
        Serial.println("Failed");
        while(true);
    }
    Serial.println("done!");
    ftpSrv.setCallback(_callback);
    ftpSrv.setTransferCallback(_transferCallback);

    ftpSrv.begin("esp8266","esp8266");    //username, password for ftp.   (default 21, 50009 for PASV)
}

void loop() {
    ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!!
    // server.handleClient();   //example if running a webserver you still need to call .handleClient();
}

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace) {
	Serial.print(">>>>>>>>>>>>>>> _callback " );
	Serial.print(ftpOperation);
	/* FTP_CONNECT,
	 * FTP_DISCONNECT,
	 * FTP_FREE_SPACE_CHANGE
	 */
	Serial.print(" ");
	Serial.print(freeSpace);
	Serial.print(" ");
	Serial.println(totalSpace);

	// freeSpace : totalSpace = x : 360

	if (ftpOperation == FTP_CONNECT) Serial.println(F("CONNECTED"));
	if (ftpOperation == FTP_DISCONNECT) Serial.println(F("DISCONNECTED"));
}

void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize) {
	Serial.print(">>>>>>>>>>>>>>> _transferCallback " );
	Serial.print(ftpOperation);
	/* FTP_UPLOAD_START = 0,
	 * FTP_UPLOAD = 1,
	 *
	 * FTP_DOWNLOAD_START = 2,
	 * FTP_DOWNLOAD = 3,
	 *
	 * FTP_TRANSFER_STOP = 4,
	 * FTP_DOWNLOAD_STOP = 4,
	 * FTP_UPLOAD_STOP = 4,
	 *
	 * FTP_TRANSFER_ERROR = 5,
	 * FTP_DOWNLOAD_ERROR = 5,
	 * FTP_UPLOAD_ERROR = 5
	 */
	Serial.print(" ");
	Serial.print(name);
	Serial.print(" ");
	Serial.println(transferredSize);
}