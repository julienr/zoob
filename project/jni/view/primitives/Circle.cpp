#include "Circle.h"
#include "lib/Math.h"

static size_t nverts;
static MGL_DATATYPE* verts;
static MGL_DATATYPE* texcoords;

void Circle::create (unsigned numSubdiv) {
  //FIXME: Create VBOs
  nverts = numSubdiv+2;
  verts = new MGL_DATATYPE[nverts*3];
  texcoords = new MGL_DATATYPE[nverts*2];

  const float angleIncr = 2.0f * M_PI / (float) numSubdiv;

  verts[0] = 0;
  verts[1] = 0;
  verts[2] = 0;
  texcoords[0] = texcoords[1] = 0;
  //texcoords[0] = texcoords[1] = 0.5f;
  float angle = 0;
  for (size_t i=1; i<nverts; i++, angle+=angleIncr) {
    verts[3 * i] = fX(cosf(angle));
    verts[3 * i + 1] = fX(sinf(angle));
    verts[3 * i + 2] = 0;

    texcoords[2 * i] = fX(1);
    texcoords[2 * i + 1] = 0;
   }
}

void Circle::draw (bool enableTexture) {
  glVertexPointer(3, MGL_TYPE, 0, verts);
  if (enableTexture)
    glTexCoordPointer(2, MGL_TYPE, 0, texcoords);
  glDrawArrays(GL_TRIANGLE_FAN, 0, nverts);
}
