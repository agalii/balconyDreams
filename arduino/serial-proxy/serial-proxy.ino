#include <SoftwareSerial.h>

#define sw_serial_rx_pin  2 // Connect this pin to TX on the esp8266
#define sw_serial_tx_pin  3 // Connect this pin to RX on the esp8266
#define esp8266_reset_pin 4 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

SoftwareSerial ESPserial(sw_serial_rx_pin, sw_serial_tx_pin);

void setup()
{
  Serial.begin(115200);     // communication with the host computer
  ESPserial.begin(115200);  // communication with esp8266

  pinMode(esp8266_reset_pin, OUTPUT);
  digitalWrite(esp8266_reset_pin, LOW);
  delay(100);
  digitalWrite(esp8266_reset_pin, HIGH);
  delay(1000);

/*
  ESPserial.println("AT+UART_CUR=9600,8,1,0,0");
  ESPserial.flush();
  ESPserial.end();
  ESPserial.begin(9600);
*/

  Serial.println("");
  Serial.println("Remember to to set Both NL & CR in the serial monitor.");
  Serial.println("Ready");
  Serial.println("");
}

void loop()
{
  if (ESPserial.available())
    Serial.write(ESPserial.read());

  if (Serial.available())
    ESPserial.write(Serial.read());
}
