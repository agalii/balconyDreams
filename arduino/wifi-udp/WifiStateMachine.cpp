#include "WifiStateMachine.h"

enum {
  WIFI_NULL = 0,
  WIFI_OFF,
  WIFI_RST_HIGH,
  WIFI_CIPSTART,
  WIFI_CIPSEND,
  WIFI_SEND_PAYLOAD,
};

WifiStateMachine::WifiStateMachine(int rxPin, int txPin, int _resetPin, const char *_server, int _port, PrepareCallbackFunc _payloadCB, Stream &_debugSerial)
{
  swSerial = new SoftwareSerial(rxPin, txPin);
  resetPin = _resetPin;
  server = _server;
  port = _port;
  payloadCB = _payloadCB;
  debugSerial = &_debugSerial;

  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, LOW);

  swSerial->begin(115200);
}

void WifiStateMachine::triggerState(int state)
{
  char buf[20];
  char command[64];
  snprintf(buf, sizeof(buf), "Entering state %d", state);
  Serial.println(buf);

  switch (state) {
    case WIFI_OFF:
      digitalWrite(resetPin, LOW);
      waitState(WIFI_RST_HIGH, 300000);
      break;
    case WIFI_RST_HIGH:
      digitalWrite(resetPin, HIGH);
      waitState(WIFI_CIPSTART, 10000);
      break;
    case WIFI_CIPSTART:
      snprintf(command, sizeof(command), "AT+CIPSTART=\"UDP\",\"%s\",%d,%d,0", server, port, port);
      swSerial->println(command);
      waitState(WIFI_CIPSEND, 100);
      break;
    case WIFI_CIPSEND:
      payload = payloadCB();
      snprintf(command, sizeof(command), "AT+CIPSEND=%d", strlen(payload) + 1);
      swSerial->println(command);
      waitState(WIFI_SEND_PAYLOAD, 100);
      break;
    case WIFI_SEND_PAYLOAD:
      swSerial->println(payload);
      waitState(WIFI_OFF, 10000);
      break;
  }
}

void WifiStateMachine::tick()
{
  if (debugSerial && swSerial->available())
    debugSerial->write(swSerial->read());

  StateMachine::tick();
}

void WifiStateMachine::reset()
{
  waitState(WIFI_OFF, 0);
}

