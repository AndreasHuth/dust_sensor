#include "lib.h"
#include "defines.h"
#include <Arduino.h>

unsigned long LastLedMillis = 0;
bool ledState = false;

void initLed() {
    pinMode(config::pins::LED_PIN, OUTPUT);
}

void LEDTask() {
    if (millis() - LastLedMillis >= 500) {
        LastLedMillis = millis();
        ledState = !ledState;
        digitalWrite(config::pins::LED_PIN, ledState);
    }
}

void initADC() {
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
}

float getLipoVoltage() {
    int raw = analogRead(config::pins::PIN_UBAT);
    // Faktor für ESP32-C3 Spannungsteiler
    return (float)raw * 0.0040293 * 0.905; 
}

void printLipoVoltage() {
    Serial.printf("Spannung: %.2fV\n", getLipoVoltage());
}
