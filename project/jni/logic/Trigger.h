#ifndef TRIGGER_H_
#define TRIGGER_H_

#include "def.h"
#include "triggers/Action.h"
#include "triggers/Condition.h"

class Trigger {
  public:
    Trigger (const Action* a, const Condition* cond)
      : action(a), condition(cond), done(false) {}

    void think (const Tile* tile) {
      if (condition->check(tile)) {
        action->execute(tile);
        done = !condition->canRepeat();
      }
    }

    bool isDone () {
      return done;
    }

  private:
    const Action* action;
    const Condition* condition;
    bool done;
};

#endif
