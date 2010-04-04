#include "Square.h"

MGL_DATATYPE verts[12] = {
    fX(-0.5), fX(-0.5), 0,
    fX(0.5), fX(-0.5), 0,
    fX(-0.5), fX(0.5), 0,
    fX(0.5), fX(0.5), 0
    /*0,0,0,
    fX(1.0), 0,0,
    0,fX(1.0),0,
    fX(1.0),fX(1.0),0*/
};

MGL_DATATYPE texCoords[8] = {
    0, fX(1),
    fX(1), fX(1),
    0,0,
    fX(1),0
};

uint8_t lineIndices[4] = {
    0,1,3,2
};


void Square::create () {
  //FIXME: Create VBOs
}

void Square::draw (bool enableTexture) {
  glVertexPointer(3, MGL_TYPE, 0, verts);
  if (enableTexture)
    glTexCoordPointer(2, MGL_TYPE, 0, texCoords);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Square::drawLine () {
  glVertexPointer(3, MGL_TYPE, 0, verts);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, lineIndices);
}
