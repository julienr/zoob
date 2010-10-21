#ifndef CONDITION_BREAK_H_
#define CONDITION_BREAK_H_

#include "Condition.h"

class ConditionBreak : public Condition {
  public:
    bool check (const Tile* tile) const {
      //FIXME: check
      return tile->getEntity() == NULL;
    }

    bool canRepeat () const {
      return false;
    }
};

#endif
