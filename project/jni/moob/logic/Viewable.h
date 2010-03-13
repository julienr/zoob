#ifndef VIEWABLE_H_
#define VIEWABLE_H_

#include "lib/Vector2.h"

struct Viewable {
  virtual ~Viewable ();
  //width/height in game coords
  virtual float getWidth () = 0;
  virtual float getHeight () = 0;
  virtual Vector2 getPosition () = 0;
};

#endif /* VIEWABLE_H_ */
