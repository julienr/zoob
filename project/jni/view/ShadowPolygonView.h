#ifndef SHADOWPOLYGONVIEW_H_
#define SHADOWPOLYGONVIEW_H_

#include "logic/ShadowPolygon.h"
#include "GLW.h"

class ShadowPolygonView {
  public:
    static void draw (const ShadowPolygon& poly) {
#if 0
       const Vector2* verts = poly.getVerts();
       MGL_DATATYPE glverts[12] = {
           fX(verts[ShadowPolygon::NEAR_0].x), fX(verts[ShadowPolygon::NEAR_0].y), 0,
           fX(verts[ShadowPolygon::FAR_0].x), fX(verts[ShadowPolygon::FAR_0].y), 0,
           fX(verts[ShadowPolygon::NEAR_1].x), fX(verts[ShadowPolygon::NEAR_1].y), 0,
           fX(verts[ShadowPolygon::FAR_1].x), fX(verts[ShadowPolygon::FAR_1].y), 0,
       };
       glVertexPointer(3, MGL_TYPE, 0, glverts);
       glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

       GLW::color(RED);
       glBegin(GL_LINES);
       glVertex3f(poly.getVert(ShadowPolygon::NEAR_0).x, poly.getVert(ShadowPolygon::NEAR_0).y, 0);
       glVertex3f(poly.getVert(ShadowPolygon::PENUMBRA_0).x, poly.getVert(ShadowPolygon::PENUMBRA_0).y, 0);
       glVertex3f(poly.getVert(ShadowPolygon::NEAR_1).x, poly.getVert(ShadowPolygon::NEAR_1).y, 0);
       glVertex3f(poly.getVert(ShadowPolygon::PENUMBRA_1).x, poly.getVert(ShadowPolygon::PENUMBRA_1).y, 0);
       glEnd();
       GLW::colorWhite();
       GLW::color(TRANSPARENT_GREY);
#else
       //FIXME: We're note calculating
       // the inner edge as in http://www.gamedev.net/reference/programming/features/2dsoftshadow/page5.asp
       // (we actually use C as the inner edge)
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

       /*GLW::color(RED);
       glBegin(GL_LINES);
       glVertex3f(poly.getVert(ShadowPolygon::NEAR_0).x, poly.getVert(ShadowPolygon::NEAR_0).y, 0);
       glVertex3f(poly.getVert(ShadowPolygon::PENUMBRA_0).x, poly.getVert(ShadowPolygon::PENUMBRA_0).y, 0);
       glVertex3f(poly.getVert(ShadowPolygon::NEAR_1).x, poly.getVert(ShadowPolygon::NEAR_1).y, 0);
       glVertex3f(poly.getVert(ShadowPolygon::PENUMBRA_1).x, poly.getVert(ShadowPolygon::PENUMBRA_1).y, 0);
       glEnd();
       GLW::colorWhite();
       GLW::color(TRANSPARENT_GREY);*/
#endif
    }
};

#endif /* SHADOWPOLYGONVIEW_H_ */
