#include <SPI.h>

#include "wifimanager.h"
#include "credentials.h"
#include "macros.h"

enum {
  WIFIMANAGER_STATE_IDLE = 0,
  WIFIMANAGER_STATE_CONNECTING,
  WIFIMANAGER_STATE_CONNECTED,
  WIFIMANAGER_STATE_UPDATE_CALLBACK,
  WIFIMANAGER_STATE_ERROR,
};

WifiManager::WifiManager(WifiManagerUpdateCallback _callback, void *_ctx) {
  callback = _callback;
  ctx = _ctx;
  waitState(WIFIMANAGER_STATE_IDLE, 0);
}

void WifiManager::connect(const char *_ssid, const char *_passphrase) {
  ssid = _ssid;
  passphrase = _passphrase;
  WiFi.begin(ssid, passphrase);
  waitState(WIFIMANAGER_STATE_CONNECTING, 0);
}

void WifiManager::disconnect() {
  WiFi.end();
}

void WifiManager::triggerState(int state) {
  switch (state) {
    case WIFIMANAGER_STATE_IDLE: {
        IPAddress localip = WiFi.localIP();
        Serial.print("LocalIP: ");
        Serial.println(localip);
        Serial.print("\n");
        waitState(WIFIMANAGER_STATE_UPDATE_CALLBACK, 10 * MILLIS_PER_SECOND);
      break;
    }

    case WIFIMANAGER_STATE_CONNECTING:
      wifiStatus = WiFi.status();
      Serial.print("WIFI connecting ...");
      Serial.print(wifiStatus, DEC);
      Serial.print("\n\r");

      if (wifiStatus == WL_CONNECTED)
        waitState(WIFIMANAGER_STATE_CONNECTED, 0);
      else
        waitState(WIFIMANAGER_STATE_CONNECTING, 500);

      break;

    case WIFIMANAGER_STATE_CONNECTED: {
      Serial.print("WIFI connected!\n\r");

      IPAddress gateway = WiFi.gatewayIP();
      Serial.print("GATEWAY: ");
      Serial.print(gateway);
      Serial.print("\n");

      IPAddress localip = WiFi.localIP();
      Serial.print("LocalIP: ");
      Serial.println(localip);
      Serial.print("\n");

      waitState(WIFIMANAGER_STATE_IDLE, 0);
      break;
    }

    case WIFIMANAGER_STATE_UPDATE_CALLBACK:
      // Just call the callback
      waitState(WIFIMANAGER_STATE_IDLE, 0);
      break;
  }

  callback(ctx);
}

bool WifiManager::connected() {
  return WiFi.status() >= WL_CONNECTED && WiFi.localIP() != INADDR_NONE;
}

char *WifiManager::getWifiSSID() {
  return WiFi.SSID();
}

long WifiManager::getWifiRSSI() {
  return WiFi.RSSI();
}

IPAddress WifiManager::getLocalIP() {
  return WiFi.localIP();
}
