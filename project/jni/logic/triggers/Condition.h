#ifndef CONDITION_H_
#define CONDITION_H_

#include "logic/Level.h"

class Condition {
  public:
    virtual bool check (const Tile* tile) const = 0;

    virtual bool canRepeat () const = 0;
};

#endif
