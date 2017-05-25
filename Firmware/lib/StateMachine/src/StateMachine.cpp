#include "StateMachine.h"
#include <limits.h>

StateMachine::StateMachine()
{
  nextState = currentState = STATEMACHINE_STATE_UNSET;
  stateTimeout = 0;
  lastMillis = stateStart = millis();
}

void StateMachine::waitState(int state, unsigned long timeout)
{
  nextState = state;
  stateTimeout = timeout;
  stateStart = millis();
}

bool StateMachine::stateTransitioning()
{
  return nextState != STATEMACHINE_STATE_UNSET;
}

unsigned long StateMachine::timeLeft()
{
  unsigned long now = millis();

  if (now > stateTimeout)
    return (ULONG_MAX - now) + stateTimeout;

  return stateTimeout - now;
}

int StateMachine::getCurrentState()
{
  return currentState;
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
  nextState = STATEMACHINE_STATE_UNSET;
  triggerState(currentState);
  stateStart = now;
}
