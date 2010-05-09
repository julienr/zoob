#ifndef SHADOWPOLYGONVIEW_H_
#define SHADOWPOLYGONVIEW_H_

#include "logic/ShadowPolygon.h"
#include "GLW.h"

class ShadowPolygonView {
  public:
    static void draw (const ShadowPolygon& poly) {
       const Vector2* verts = poly.getVerts();
       MGL_DATATYPE glverts[18] = {
           fX(verts[ShadowPolygon::PENUMBRA_0].x), fX(verts[ShadowPolygon::PENUMBRA_0].y), 0,
           fX(verts[ShadowPolygon::NEAR_0].x), fX(verts[ShadowPolygon::NEAR_0].y), 0,
           fX(verts[ShadowPolygon::FAR_0].x), fX(verts[ShadowPolygon::FAR_0].y), 0,
           fX(verts[ShadowPolygon::NEAR_1].x), fX(verts[ShadowPolygon::NEAR_1].y), 0,
           fX(verts[ShadowPolygon::FAR_1].x), fX(verts[ShadowPolygon::FAR_1].y), 0,
           fX(verts[ShadowPolygon::PENUMBRA_1].x), fX(verts[ShadowPolygon::PENUMBRA_1].y), 0,
       };
       MGL_DATATYPE texcoords[12] = {
           fX(0), fX(1),
           fX(0), fX(0),
           fX(1), fX(1),
           fX(0), fX(0),
           fX(1), fX(1),
           fX(0), fX(1)
       };
       glVertexPointer(3, MGL_TYPE, 0, glverts);
       glTexCoordPointer(2, MGL_TYPE, 0, texcoords);
       glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
    }
};

#endif /* SHADOWPOLYGONVIEW_H_ */
