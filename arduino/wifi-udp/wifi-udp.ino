#include <SoftwareSerial.h>
#include <limits.h>

#define sw_serial_rx_pin 2  //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin 3  //  Connect this pin to RX on the esp8266
#define esp8266_reset_pin 4 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

#define SERVER_IP "192.168.178.131"
#define SERVER_PORT 2121

enum {
  WIFI_NULL,
  WIFI_OFF,
  WIFI_RST_HIGH,
  WIFI_CIPSTART,
  WIFI_CIPSEND,
  WIFI_SEND_PAYLOAD,
  WIFI_LAST,
};

int wifiState;
unsigned long wifiStateStart;
unsigned long wifiStateTimeout;
char wifiPacketPayload[32];
unsigned long prevMillis;

SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);

void setup() {
  pinMode(esp8266_reset_pin, OUTPUT);
  digitalWrite(esp8266_reset_pin, LOW);

  Serial.begin(115200);
  swSerial.begin(115200);

  wifiState = WIFI_NULL;
  prevMillis = wifiStateStart = millis();
  wifiStateTimeout = 0;
}

void preparePayload() {
  static int seq = 0;
  snprintf(wifiPacketPayload, sizeof(wifiPacketPayload), "Seq %d, Battery %d\n", seq++, analogRead(0));
}

void wifiTick() {
  unsigned long newTimeout = 0;
  char command[64];
  unsigned long currentMillis = millis();

  // millisecond counter overflow. happens about every 53 days
  if (currentMillis < prevMillis) {
    wifiStateTimeout -= ULONG_MAX - wifiStateStart;
    wifiStateStart = 0;
  }

  prevMillis = currentMillis;

  // if there is an overflow in the addition, simply bail out and wait for the overflow to happen
  if (wifiStateStart + wifiStateTimeout < wifiStateStart)
    return;
  
  if (currentMillis < wifiStateStart + wifiStateTimeout)
    return;

  // state expired, increment
  wifiState++;

  char buf[20];
  snprintf(buf, sizeof(buf), "Entering state %d", wifiState);
  Serial.println(buf);

  switch (wifiState) {
    case WIFI_OFF:
      digitalWrite(esp8266_reset_pin, LOW);
      newTimeout = 60000;
      break;
    case WIFI_RST_HIGH:
      digitalWrite(esp8266_reset_pin, HIGH);
      newTimeout = 10000;
      break;
    case WIFI_CIPSTART:
      snprintf(command, sizeof(command), "AT+CIPSTART=\"UDP\",\"%s\",%d,%d,0", SERVER_IP, SERVER_PORT, SERVER_PORT);
      swSerial.println(command);
      newTimeout = 100;
      break;
    case WIFI_CIPSEND:
      preparePayload();
      snprintf(command, sizeof(command), "AT+CIPSEND=%d", strlen(wifiPacketPayload) + 1);
      swSerial.println(command);
      newTimeout = 100;
      break;
    case WIFI_SEND_PAYLOAD:
      swSerial.println(wifiPacketPayload);
      newTimeout = 100;
      break;
    case WIFI_LAST:
      wifiState = WIFI_NULL;
      break;
  }

  wifiStateTimeout = newTimeout;
  wifiStateStart = currentMillis;
}

void loop() {
  if (swSerial.available())
    Serial.write(swSerial.read());

  wifiTick();
}
