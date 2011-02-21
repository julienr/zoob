#ifndef ACTION_H_
#define ACTION_H_

#include "zoobdef.h"
#include "logic/Level.h"

class Action {
  public:
    virtual void execute (const Tile* tile) const = 0;
};

#endif
