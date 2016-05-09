#include "WifiStateMachine.h"

#define sw_serial_rx_pin  2  // Connect this pin to TX on the esp8266
#define sw_serial_tx_pin  3  // Connect this pin to RX on the esp8266
#define esp8266_reset_pin 4  // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

#define SERVER_IP "192.168.178.131"
#define SERVER_PORT 2121

static char wifiPacketPayload[32];

static const char *preparePayload(void) {
  static int seq = 0;
  snprintf(wifiPacketPayload, sizeof(wifiPacketPayload), "Seq %d, Battery %d\n", seq++, analogRead(0));
  return wifiPacketPayload;
}

WifiStateMachine wifi(sw_serial_rx_pin, sw_serial_tx_pin, esp8266_reset_pin,
                      SERVER_IP, SERVER_PORT, preparePayload, Serial);

void setup() {
  Serial.begin(115200);
  wifi.reset();
}

void loop() {
  wifi.tick();
}
