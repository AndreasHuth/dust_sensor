#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "defines.h"
#include "lib.h"

// Instanzen
AsyncWebServer server(80);
WebSocketsServer webSocket(81);

// Sensor Status
uint32_t sensorTimer = 0;
bool sensorActive = false;
bool warmUp = false;

void setup() {
    Serial.begin(115200);
// Warte bis zu 5 Sekunden, ob eine USB-Verbindung steht
    while (!Serial && millis() < 5000); 
    
    Serial.println("--- System Start ---");
    Serial.println("booting ...");

    // Initialisierung der Funktionen aus lib.cpp
    initLed();
    initADC();
    
    pinMode(config::pins::MOSFET_PIN, OUTPUT);
    digitalWrite(config::pins::MOSFET_PIN, HIGH); // Sensor initial AUS

    if(!LittleFS.begin(true)) Serial.println("LittleFS Error");

    // WiFi & WebServer
    WiFi.softAP("Luft-Monitor", "12345678");
    webSocket.begin();
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });
    server.begin();

    // PMS5003 an Serial1
    Serial1.begin(9600, SERIAL_8N1, config::pins::PMS_RX, config::pins::PMS_TX);
    
    sensorTimer = millis();
}

void loop() {
    LEDTask();
    webSocket.loop();

    uint32_t now = millis();

    // Logik: 50s Messung inkl. 30s Warmup -> 10s Pause
    if (!sensorActive && (now - sensorTimer >= 10000)) {
        digitalWrite(config::pins::MOSFET_PIN, LOW); // EIN
        sensorActive = true;
        warmUp = true;
        sensorTimer = now;
        Serial.println("Sensor heizt auf...");
    } 
    else if (sensorActive) {
        if (warmUp && (now - sensorTimer >= 30000)) {
            warmUp = false;
        } 
        else if (!warmUp) {
            // Daten lesen
            if (Serial1.available() >= 32) {
                uint8_t buf[32];
                Serial1.readBytes(buf, 32);
                if (buf[0] == 0x42 && buf[1] == 0x4D) {
                    // Validate PMS5003 checksum (sum of bytes 0..29 == checksum at 30..31)
                    uint16_t recv_checksum = ((uint16_t)buf[30] << 8) | buf[31];
                    uint16_t calc = 0;
                    for (int i = 0; i < 30; ++i) calc += buf[i];
                    if (calc != recv_checksum) {
                        Serial.println("PMS checksum mismatch, skipping frame");
                    } else {
                        StaticJsonDocument<256> doc;
                        doc["pm25"] = (buf[12] << 8) | buf[13];
                        doc["vbat"] = getLipoVoltage();

                        String out;
                        serializeJson(doc, out);
                        webSocket.broadcastTXT(out);
                    }
                }
            }

            if (now - sensorTimer >= 50000) {
                digitalWrite(config::pins::MOSFET_PIN, HIGH); // AUS
                sensorActive = false;
                sensorTimer = now;
            }
        }
    }
}