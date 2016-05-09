#include <Arduino.h>

#pragma once

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
  virtual void tick();
};
