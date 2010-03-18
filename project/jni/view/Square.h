#ifndef SQUARE_H_
#define SQUARE_H_

#include "def.h"

class Square {
  public:
    static void create ();

    static void draw (bool enableTexture);
  private:
    static int verts[12];
    static int texCoords[8];
};



#endif /* SQUARE_H_ */
