#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "defines.h"

// Globale Variablen für Netzwerkstatus
bool connected = false;

class NetworkManager {
public:
    // Lädt die Konfiguration aus /config.json und gibt true bei Erfolg zurück
    bool load_config() {
        // Expect LittleFS to be mounted by the main setup()
        if (!LittleFS.exists("/config.json")) {
            Serial.println("config.json not found or LittleFS not mounted");
            return false;
        }

        File file = LittleFS.open("/config.json", "r");
        if (!file) {
            Serial.println("Failed to open /config.json");
            return false;
        }

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            Serial.print("Failed to parse config.json: ");
            Serial.println(error.c_str());
            return false;
        }

        const char* ssid = doc["SSID"] | "ESP32-Station";
        const char* password = doc["PASSWORD"] | "";

        Serial.print("Config geladen für: ");
        Serial.println(ssid);

        // Hier könnte man die Werte in Klassenvariablen speichern
        return true;
    }
};

// Initialisiert Access Point; um Namen/Kennwort zu überschreiben, Parameter übergeben
void initWiFiAP(const char* ap_ssid = "Feinstaub-Station", const char* ap_pass = "12345678") {
    WiFi.mode(WIFI_AP);
    bool ok = WiFi.softAP(ap_ssid, ap_pass);
    connected = ok;

    Serial.print("Started AP '");
    Serial.print(ap_ssid);
    Serial.print("' -> ");
    Serial.println(ok ? "OK" : "FAILED");
}

// Kompatibilitäts-Wrapper für vorhandene C-Schnittstelle
void init() {
    initWiFiAP();
}

// Platzhalter, falls alte API noch verwendet wird
void serverLoop() {
    // Falls später ein Webserver-loop benötigt wird, implementieren
}