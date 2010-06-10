#ifndef TIMER_H_
#define TIMER_H_

#include "TimerManager.h"

class Timer {
  friend class TimerManager;
  public:
    Timer(double period)
      : period(period), active(false), timeLeft(0)  {
      TimerManager::getInstance()->registerTimer(this);
    }

    ~Timer () {
      TimerManager::getInstance()->unregisterTimer(this);
    }

    void start () {
      timeLeft = period;
      active = true;
    }

    void stop () {
      active = false;
    }

    bool isActive () const {
      return active;
    }

    double getTimeLeft () const {
      return timeLeft;
    }

  protected:
    void think (double elapsedS) {
      timeLeft -= elapsedS;
      if (timeLeft < 0) {
        active = false;
        timeLeft = 0;
      }
    }
  private:
    const double period;
    bool active;
    double timeLeft;
};

#endif /* TIMER_H_ */
