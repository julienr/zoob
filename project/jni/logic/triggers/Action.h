#ifndef ACTION_H_
#define ACTION_H_

#include "def.h"
#include "logic/Level.h"

class Action {
  public:
    virtual void execute (const Tile* tile) const = 0;
};

#endif
