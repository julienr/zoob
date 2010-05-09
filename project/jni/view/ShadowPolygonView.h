#ifndef SHADOWPOLYGONVIEW_H_
#define SHADOWPOLYGONVIEW_H_

#include "logic/ShadowPolygon.h"
#include "GLW.h"

class ShadowPolygonView {
  public:
    static void draw (const ShadowPolygon& poly) {
      glPointSize(3.0f);
       GLW::color(RED);
       GLW::disableTextures();
       const Vector2* verts = poly.getVerts();
       MGL_DATATYPE glverts[12] = {
           fX(verts[0].x), fX(verts[0].y), 0,
           fX(verts[1].x), fX(verts[1].y), 0,
           fX(verts[3].x), fX(verts[3].y), 0,
           fX(verts[2].x), fX(verts[2].y), 0
       };
       glVertexPointer(3, MGL_TYPE, 0, glverts);
       glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
       GLW::enableTextures();
       GLW::colorWhite();
    }
};

#endif /* SHADOWPOLYGONVIEW_H_ */
