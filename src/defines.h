#ifndef DEFINES_H_
#define DEFINES_H_

#include <Arduino.h>

namespace config {
    namespace pins {
        const uint8_t LED_PIN = 8;          // Onboard LED
        const uint8_t BUZZER_PIN = 3;       // Buzzer
        const uint8_t PIN_UBAT = 0;         // LiPo Messung (GPIO 0)
        const uint8_t MOSFET_PIN = 7;       // Sensor Power
        const uint8_t PMS_RX = 4;           // PMS5003 TX -> ESP RX
        const uint8_t PMS_TX = 5;           // ESP TX -> PMS RX
    }
}

#define DEBUG
#endif  // DEFINES_H_