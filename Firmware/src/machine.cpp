#include "machine.h"
#include "pins.h"
#include "macros.h"

#define ANALOG_RESOLUTION 12

enum {
  MACHINE_STATE_INITIALIZING = 0,
  MACHINE_STATE_IDLE,
  MACHINE_STATE_MEASURE,
  MACHINE_STATE_WATER,
};

#define MEASURE_INTERVAL (30 * MILLIS_PER_MINUTE)
#define WATER_DURATION (45 * MILLIS_PER_SECOND)
#define WATER_THRESHOLD 3000
#define LOG_SERVER_PORT 2121

struct sensorValveMapping {
  int sensor;
  int valve;
} sensorValveMappings[] = {
  { .sensor = 1, .valve = 4, },
  { .sensor = 1, .valve = 5, },
  { .sensor = 1, .valve = 6, },
//  { .sensor = 2, .valve = 1, },
  { .sensor = 3, .valve = 3, },
  { .sensor = 4, .valve = 2, },
};

// Static callback wrappers

static void staticStatusTimerCallback(void *ctx) {
  Machine *machine = (Machine *) ctx;
  machine->statusTimerCallback();
}

static void staticDebugTimerCallback(void *ctx) {
  Machine *machine = (Machine *) ctx;
  machine->debugTimerCallback();
}

static void staticWifiManagerCallback(void *ctx) {
  Machine *machine = (Machine *) ctx;
  machine->wifiManagerCallback();
}

void Machine::startWater(unsigned int valve) {
  if (valve < ARRAY_SIZE(valvePins)) {
    digitalWrite(PIN_PUMP, 1);
    digitalWrite(valvePins[valve], 1);
  }
}

void Machine::stopWater() {
  digitalWrite(PIN_PUMP, 0);

  for (unsigned int i = 0; i < ARRAY_SIZE(valvePins); i++)
    digitalWrite(valvePins[i], 0);
}

void Machine::setLED(unsigned int led, bool state) {
  if (led < ARRAY_SIZE(ledPins))
    digitalWrite(ledPins[led], state);
}

void Machine::debugTimerCallback() {
  uptime++;

#if 0
  debugLog("debug\n");

  static int state = 1;

  stopWater();
  startWater(state);

  state++;
  if (state > 7)
    state = 0;
#endif
}

void Machine::statusTimerCallback() {
  char buf[256];

  xsprintf(buf, "status: { uptime: %lu, valves_watered: [ %d, %d, %d, %d, %d, %d, %d, %d ]}\n", uptime,
           valveWateredCount[0], valveWateredCount[1], valveWateredCount[2], valveWateredCount[3],
           valveWateredCount[4], valveWateredCount[5], valveWateredCount[6], valveWateredCount[7]);
  debugLog(buf);
}

void Machine::wifiManagerCallback() {
  setLED(0, wifiManager->connected());

  if (!wifiManager->connected()) {
    wifiManager->disconnect();
    wifiManager->connect(WIFI_SSID, WIFI_PASS);
  }
}

Machine::Machine() {
  uptime = 0;

  memset(valvePending, 0, sizeof(valvePending));
  memset(valveWateredCount, 0, sizeof(valveWateredCount));
}

void Machine::initialize() {
  unsigned int i;

  analogReadResolution(ANALOG_RESOLUTION);

  pinMode(PIN_PUMP, OUTPUT);
  digitalWrite(PIN_PUMP, 0);

  for (i = 0; i < ARRAY_SIZE(valvePins); i++) {
    pinMode(valvePins[i], OUTPUT);
    digitalWrite(valvePins[i], 0);
  }

  for (i = 0; i < ARRAY_SIZE(ledPins); i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], 0);
  }

  statusTimer = new Timer(10 * MILLIS_PER_MINUTE, staticStatusTimerCallback, this);
  debugTimer = new Timer(1 * MILLIS_PER_SECOND, staticDebugTimerCallback, this);

  wifiManager = new WifiManager(staticWifiManagerCallback, this);
  wifiManager->connect(WIFI_SSID, WIFI_PASS);

  webServer = new WiFiServer(80);
  webServer->begin();

#ifdef UDP_LOG_ADDRESS
  logServer = new WiFiUDP();
  logServerAddress = IPAddress(UDP_LOG_ADDRESS);
  logServer->begin(LOG_SERVER_PORT);
#endif

  waitState(MACHINE_STATE_IDLE, 0);
  debugLog("Machine initialized. Entering main loop.\n");
}

void Machine::checkWebServer() {
  WiFiClient client;

  if ((client = webServer->available())) {
    setLED(1, 1);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");  // the connection will be closed after completion of the response
    client.println("Refresh: 1");  // refresh the page automatically every 5 sec
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");

    // output the value of each analog input pin
    for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
      int sensorReading = analogRead(analogChannel);
      client.print("analog input ");
      client.print(analogChannel);
      client.print(" is ");
      client.print(sensorReading);
      client.println("<br />");
    }

    client.println("<br />");
    client.println("Uptime: ");
    client.println(uptime);
    client.println("seconds<br />");

    client.println("</html>");
    client.stop();

    setLED(1, 0);
  }
}

void Machine::triggerState(int state) {
  switch (state) {
    case MACHINE_STATE_IDLE:
      stopWater();
      waitState(MACHINE_STATE_MEASURE, MEASURE_INTERVAL);
      break;

    case MACHINE_STATE_MEASURE: {
      unsigned int sensorReading[6];

      for (unsigned int i = 0; i < ARRAY_SIZE(sensorReading); i++)
        sensorReading[i] = analogRead(i);

      for (unsigned int i = 0; i < ARRAY_SIZE(sensorValveMappings); i++) {
        struct sensorValveMapping *map = sensorValveMappings + i;

        if (sensorReading[map->sensor - 1] > WATER_THRESHOLD)
          valvePending[map->valve - 1] = true;
      }

      char buf[256];
      xsprintf(buf, "measurements: { sensors: [ %d, %d, %d, %d, %d, %d ], valves_pending: [ %d, %d, %d, %d, %d, %d, %d ] }\n",
              sensorReading[0], sensorReading[1], sensorReading[2], sensorReading[3],
              sensorReading[4], sensorReading[5],
              valvePending[0], valvePending[1], valvePending[2], valvePending[3],
              valvePending[4], valvePending[5], valvePending[6]);
      debugLog(buf);

      waitState(MACHINE_STATE_WATER, 0);

      break;
    }

    case MACHINE_STATE_WATER:
      for (unsigned int i = 0; i < ARRAY_SIZE(valvePending); i++) {
        if (valvePending[i]) {
          stopWater();
          startWater(i);
          valvePending[i] = false;

          char buf[256];
          xsprintf(buf, "Watering %d\n", i+1);
          debugLog(buf);

          valveWateredCount[i]++;

          waitState(MACHINE_STATE_WATER, WATER_DURATION);
          return;
        }
      }

      waitState(MACHINE_STATE_IDLE, 0);
  }
}

void Machine::debugLog(const char *msg) {
  Serial.print(msg);

  if (logServer) {
    setLED(2, 1);
    logServer->beginPacket(logServerAddress, 2121);
    logServer->write(msg);
    logServer->endPacket();
    setLED(2, 0);
  }
}

void Machine::tick() {
  checkWebServer();

  debugTimer->tick();
  statusTimer->tick();
  wifiManager->tick();
  StateMachine::tick();
}
