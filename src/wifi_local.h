#ifndef WIFI_LOCAL_H_
#define WIFI_LOCAL_H_

#include <Arduino.h>

extern bool connected;

void init();
void serverLoop();

#endif // WIFI_LOCAL_H_
