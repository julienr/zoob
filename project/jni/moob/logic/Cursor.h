#ifndef CURSOR_H_
#define CURSOR_H_

#include "Moveable.h"

class Cursor : public Moveable {
  public:
    float getWidth () const {
      return 1.0f;
    }

    float getHeight () const {
      return 1.0f;
    }
};

#endif /* CURSOR_H_ */
