#ifndef TRIGGER_H_
#define TRIGGER_H_

#include "def.h"
#include "triggers/Action.h"
#include "triggers/Condition.h"

class Trigger {
  public:
    Trigger (const Action* a, const Condition* cond)
      : action(a), condition(cond) {}

    //execute the trigger and returns true if the trigger can be deleted (ie it was a one-time trigger)
    bool think (const Tile* tile) const {
      if (condition->check(tile)) {
        LOGE("trigger action true");
        action->execute(tile);
        return !condition->canRepeat();
      }
      return false;
    }

  private:
    const Action* action;
    const Condition* condition;
};

#endif
