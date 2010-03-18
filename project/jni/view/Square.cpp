#include "Square.h"

int Square::verts[12] = {
    fX(-0.5), fX(-0.5), 0,
    fX(0.5), fX(-0.5), 0,
    fX(-0.5), fX(0.5), 0,
    fX(0.5), fX(0.5), 0
    /*0,0,0,
    fX(1.0), 0,0,
    0,fX(1.0),0,
    fX(1.0),fX(1.0),0*/
};

int Square::texCoords[8] = {
    0, fX(1),
    fX(1), fX(1),
    0,0,
    fX(1),0
};


void Square::create () {
  //FIXME: Create VBOs
}

void Square::draw (bool enableTexture) {
  glVertexPointer(3, GL_FIXED, 0, verts);
  if (enableTexture)
    glTexCoordPointer(2, GL_FIXED, 0, texCoords);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
