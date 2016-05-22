const int PUMP = 5;
const int VALVE[] = { 6, 7, 8, 9 };

#define NUM_VALVES (sizeof(VALVE) / sizeof(VALVE[0]))

void set_valve_exclusive(int valve) {
  for (int i = 0; i < NUM_VALVES; i++)
    digitalWrite(VALVE[i], valve == i ? LOW : HIGH);  
}

void set_pump(int on) {
  digitalWrite(PUMP, on ? LOW : HIGH);
}

void setup() {
  pinMode(PUMP, OUTPUT);

  for (int i = 0; i < NUM_VALVES; i++)
    pinMode(VALVE[i], OUTPUT);

  digitalWrite(PUMP, HIGH);
  set_valve_exclusive(-1);
}

void loop() {
  set_pump(1);

  for (int i = 0; i < 2; i++) {
    set_valve_exclusive(0);
    delay(40000);
  
    set_valve_exclusive(1);
    delay(35000);
  }

  set_valve_exclusive(2);
  delay(25000);
  
  set_valve_exclusive(3);
  delay(30000);

  set_pump(0);
  set_valve_exclusive(-1);

  for (int i = 0; i < 8; i++)
    delay(3600UL * 1000UL);
}

