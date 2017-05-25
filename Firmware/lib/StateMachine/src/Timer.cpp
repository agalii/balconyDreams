#include "Timer.h"

enum {
  TIMER_STATE_IDLE = 0,
  TIMER_STATE_FIRE,
};

Timer::Timer(unsigned long _interval, TimerCallback cb, void *ctx) {
  callback = cb;
  callbackContext = ctx;
  interval = _interval;

  waitState(TIMER_STATE_IDLE, 0);
}

void Timer::triggerState(int state) {
  switch (state) {
    case TIMER_STATE_IDLE:
      waitState(TIMER_STATE_FIRE, interval);
      break;
    case TIMER_STATE_FIRE:
      fire();
      waitState(TIMER_STATE_IDLE, 0);
      break;
  }
}
