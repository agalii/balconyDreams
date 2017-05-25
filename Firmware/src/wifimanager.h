#include <Arduino.h>
#include <IPAddress.h>
#include <StateMachine.h>
#include <WiFi101.h>

#ifndef _SERVER_CONNECTION_H
#define _SERVER_CONNECTION_H

typedef void (*WifiManagerUpdateCallback)(void *ctx);

class WifiManager : public StateMachine {
private:
  WifiManagerUpdateCallback callback;
  void *ctx;

  const char *ssid, *passphrase;
  int wifiStatus;
  void triggerState(int state);

public:
  WifiManager(WifiManagerUpdateCallback callback, void *ctx);
  void connect(const char *ssid, const char *passphrase);
  void disconnect();

  bool connected();
  char *getWifiSSID();
  long getWifiRSSI();
  IPAddress getLocalIP();
};

#endif /* end of include guard: _SERVER_CONNECTION_H */
