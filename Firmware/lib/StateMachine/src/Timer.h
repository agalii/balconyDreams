#include <StateMachine.h>

#ifndef _TIMER_H
#define _TIMER_H

typedef void (*TimerCallback)(void *ctx);

class Timer : public StateMachine {
private:
  TimerCallback callback;
  void *callbackContext;
  unsigned long interval;

  void triggerState(int state);

protected:
  virtual void fire() { callback(callbackContext); };

public:
  Timer(unsigned long _interval, TimerCallback callback, void *callbackContext);
};

#endif /* end of include guard: _TIMER_H */
