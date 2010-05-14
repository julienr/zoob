#ifndef SHADOWPOLYGONVIEW_H_
#define SHADOWPOLYGONVIEW_H_

#include "logic/ShadowPolygon.h"
#include "GLW.h"

class ShadowPolygonView {
  public:
    static void draw (const ShadowPolygon* poly);

    static void drawLine (const ShadowPolygon* poly, const Line* line,
                            ShadowPolygon::eVert from, ShadowPolygon::eVert to);

    static void debugDraw (const ShadowPolygon* poly);
};

#endif /* SHADOWPOLYGONVIEW_H_ */
