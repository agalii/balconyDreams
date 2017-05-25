#include <Arduino.h>
#include "machine.h"

static Machine machine;

// Arduino framework entry points

void setup() {
  Serial.begin(9600);
  machine.initialize();
}

void loop() {
  machine.tick();
}
