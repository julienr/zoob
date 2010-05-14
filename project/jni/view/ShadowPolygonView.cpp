#include "ShadowPolygonView.h"

void ShadowPolygonView::draw (const ShadowPolygon* poly) {
   const Vector2* verts = poly->getVerts();
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

void ShadowPolygonView::drawLine (const ShadowPolygon* poly, const Line* line,
                        ShadowPolygon::eVert from, ShadowPolygon::eVert to) {
  const Vector2* verts = poly->getVerts();
  MGL_DATATYPE glverts[6] = {
      fX(verts[from].x), fX(verts[from].y),0,
      fX(verts[to].x), fX(verts[to].y), 0
  };

  glVertexPointer(3, MGL_TYPE, 0, glverts);
  glDrawArrays(GL_LINES, 0, 2);

  const Vector2 center = (verts[to]+verts[from])/2.0f;
  const Vector2 nto = center + line->getNormal();
  MGL_DATATYPE nverts[6] = {
      fX(center.x), fX(center.y), 0,
      fX(nto.x), fX(nto.y), 0
  };

  glVertexPointer(3, MGL_TYPE, 0, nverts);
  glDrawArrays(GL_LINES, 0, 2);
}

void ShadowPolygonView::debugDraw (const ShadowPolygon* poly) {
  const Line* const* lines = poly->getLines();
  /*glPointSize(5.0f);
  const Vector2* verts = poly->getVerts();
  GLW::color(BLACK);
  glBegin(GL_POINTS);
    glVertex3f(verts[ShadowPolygon::NEAR_0].x, verts[ShadowPolygon::NEAR_0].y, 0);
  glEnd();
  GLW::colorWhite();
  glPointSize(1.0f);*/
  drawLine (poly, lines[0], ShadowPolygon::NEAR_0, ShadowPolygon::PENUMBRA_0);
  drawLine (poly, lines[1], ShadowPolygon::PENUMBRA_0, ShadowPolygon::PENUMBRA_1);
  drawLine (poly, lines[2], ShadowPolygon::PENUMBRA_1, ShadowPolygon::NEAR_1);
  drawLine (poly, lines[3], ShadowPolygon::NEAR_1, ShadowPolygon::NEAR_0);
}
