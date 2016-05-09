#include <Arduino.h>
#include <SoftwareSerial.h>
#include "StateMachine.h"

#pragma once

typedef const char * (*PrepareCallbackFunc)(void);

class WifiStateMachine : public StateMachine
{
private:
  SoftwareSerial *swSerial;
  void triggerState(int state);

  PrepareCallbackFunc payloadCB;
  Stream *debugSerial;

  const char *payload;
  const char *server;
  int resetPin;
  int port;

public:
  WifiStateMachine(int rxPin, int txPin, int resetPin, const char *server, int port, PrepareCallbackFunc payloadCB, Stream &serial);
  void tick();
  void reset();
};
