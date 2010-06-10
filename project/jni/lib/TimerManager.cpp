#include "TimerManager.h"
#include "Timer.h"

TimerManager* TimerManager::instance = NULL;

void TimerManager::tick (double elapsedS) {
  LIST_FOREACH(Timer*,timers,t) {
    (*t)->think(elapsedS);
  }
}
