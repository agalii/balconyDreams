#include <Arduino.h>

#pragma once

#define MILLIS_PER_SECOND  1000UL
#define MILLIS_PER_MINUTE  (60UL * MILLIS_PER_SECOND)
#define MILLIS_PER_HOUR    (60UL * MILLIS_PER_MINUTE)

class StateMachine
{
protected:
  virtual void reset(int state) {};
  virtual void triggerState(int state) {};
  void waitState(int state, unsigned long timeout);

private:
  unsigned long stateStart;
  unsigned long stateTimeout;
  unsigned long lastMillis;

  int currentState;
  int nextState;

public:
  StateMachine();
  void tick();
};
