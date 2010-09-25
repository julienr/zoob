#ifndef CIRCLE_H_
#define CIRCLE_H_

#include "def.h"

class Circle {
  public:
    static void create (unsigned numSubdiv=20);

    static void draw (bool enableTexture);
};

#endif
