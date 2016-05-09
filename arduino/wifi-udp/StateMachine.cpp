#include "StateMachine.h"
#include <limits.h>

StateMachine::StateMachine()
{
  nextState = currentState = -1;
  stateTimeout = 0;
  lastMillis = stateStart = millis();
}

void StateMachine::waitState(int state, unsigned long timeout)
{
  nextState = state;
  stateTimeout = timeout;
}

void StateMachine::tick()
{
  unsigned long now;
  
  if (nextState < 0)
    return;

  now = millis();

  // millisecond counter overflow. happens about every 53 days
  if (now < lastMillis) {
    stateTimeout -= ULONG_MAX - stateStart;
    stateStart = 0;
  }

  lastMillis = now;

  // if there is an overflow in the addition, simply bail out and wait for the overflow to happen
  if (stateStart + stateTimeout < stateStart)
    return;

  if (now < stateStart + stateTimeout)
    return;

  currentState = nextState;
  triggerState(currentState);
  stateStart = now;
}

