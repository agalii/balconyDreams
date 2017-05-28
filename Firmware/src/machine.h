#include <Arduino.h>
#include <StateMachine.h>
#include <Timer.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

#include "wifimanager.h"
#include "credentials.h"

#pragma once

class Machine : public StateMachine {

private:
  Timer *debugTimer;
  Timer *statusTimer;
  Timer *measurementTimer;

  WifiManager *wifiManager;
  WiFiServer *webServer;
  WiFiUDP *logServer;
  IPAddress logServerAddress;

  unsigned long uptime;

  unsigned int valvesDuration[8];
  unsigned int valveWateredCount[8];

  void triggerState(int state);
  void startWater(unsigned int valve);
  void stopWater();
  void setLED(unsigned int led, bool state);
  void checkWebServer();

public:
  Machine();
  void initialize();
  void tick();

  // Must be public for static wrappers
  void debugTimerCallback();
  void statusTimerCallback();
  void measurementTimerCallback();
  void wifiManagerCallback();

  void debugLog(const char[]);
};
