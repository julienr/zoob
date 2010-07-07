#ifndef TIMERMANAGER_H_
#define TIMERMANAGER_H_

#include "containers/list.h"

class Timer;

class TimerManager {
  /** Singleton */
  private:
    static TimerManager* instance;
    TimerManager () {}

  public:
    static TimerManager* getInstance () {
      if (!instance)
        instance = new TimerManager();
      return instance;
    }

    static void destroy () {
      delete instance;
    }

  /** Instance method */
  public:
    void registerTimer (Timer* t) {
      timers.append(t);
    }

    void unregisterTimer (Timer* t) {
      timers.remove(t);
    }

    void tick (double elapsedS);


  private:
    list<Timer*> timers;

};

#endif /* TIMERMANAGER_H_ */
