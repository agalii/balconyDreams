#include "WifiStateMachine.h"

#define sw_serial_rx_pin  2  // Connect this pin to TX on the esp8266
#define sw_serial_tx_pin  3  // Connect this pin to RX on the esp8266
#define esp8266_reset_pin 4  // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

#define SERVER_IP "192.168.178.131"
#define SERVER_PORT 2121

static char wifiPacketPayload[64];
static unsigned int pumpTime = 0;

static const char *preparePayload(void) {
  static int seq = 0;
  snprintf(wifiPacketPayload, sizeof(wifiPacketPayload), "Seq %d, Battery %d, Tank %d, PumpTime %d\n", seq++, analogRead(0), analogRead(1), pumpTime);
  pumpTime = 0;
  return wifiPacketPayload;
}

WifiStateMachine wifi(sw_serial_rx_pin, sw_serial_tx_pin, esp8266_reset_pin,
                      SERVER_IP, SERVER_PORT, preparePayload, Serial);

class PumpStateMachine : public StateMachine
{
private:
  void triggerState(int state) {
    switch (state) {
      case 0:
        Serial.println("pump off");
        digitalWrite(12, HIGH);
        digitalWrite(5, HIGH);
        waitState(1, 1 * MILLIS_PER_MINUTE);
        break;
      case 1:
        Serial.println("pump on");
        digitalWrite(12, LOW);
        digitalWrite(5, LOW);
        waitState(0, 30 * MILLIS_PER_SECOND);
        pumpTime += 30 * MILLIS_PER_SECOND;
        break;
    }
  }
public:
  PumpStateMachine() {
    waitState(0, 0);
  };
};

PumpStateMachine pump;

void setup() {
  Serial.begin(115200);
  wifi.reset();

  pinMode(5, OUTPUT);
  pinMode(12, OUTPUT);

  digitalWrite(5, HIGH);
  digitalWrite(12, HIGH);
}

void loop() {
  wifi.tick();
  pump.tick();
}
