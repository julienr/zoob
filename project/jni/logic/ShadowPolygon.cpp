#include "ShadowPolygon.h"
#include "logic/physics/CollisionManager.h"
#include "logic/Level.h"
#include "lib/Line.h"

//We're using a game screen resolution of 10/15, so 20 will just project our shadows out screen
#define FARDIST 15

void ShadowPolygon::_castShadow (const Vector2& lightSource,
                                     const AABBox* bbox,
                                     const Vector2& bboxPos) {
  Vector2 corners[4];
  bbox->getCorners(bboxPos, corners);

  int numFound = 0;
  const Vector2 n = corners[0]-corners[3];
  bool prevFacingLight = (corners[0]-lightSource)*n > 0;
  //LOGE("silhouette ");
  for (int i=0; i<4; i++) {
    const int next = (i+1)%4;
    const Vector2 n = corners[next]-corners[i];
    const float dot = (corners[next]-lightSource)*n;
    //LOGE("%i : dot : %f", next, dot);
    const bool facingLight = dot > 0;

    if (facingLight != prevFacingLight) {
      if (numFound == 0)
        verts[NEAR_0] = corners[next];
      else if (numFound == 1) {
        verts[NEAR_1] = corners[next];
        return;
      }
      numFound++;
    }
    prevFacingLight = facingLight;
  }
  //Should never reach this point, should always find 2 vertices
  ASSERT(false);
}

bool ShadowPolygon::inside (const Vector2& p) const {
  for (int i=0; i<4; i++) {
    if (lines[i]->getRelativePosition(p) > 0)
      return false;
  }
  return true;
}

eRelativePos ShadowPolygon::classifyCircle (const Vector2& center, float r) const {
  bool inside = true;
  for (int i=0; i<4; i++) {
    const float d = lines[i]->distance(center);
    if (d > r)
      return OUTSIDE;
    else if (d > -r)
      inside = false;
  }
  return inside?INSIDE:INTERSECT;
}

void ShadowPolygon::_calculateFar (const Vector2& lightSource,
                                       eVert far) {
  const eVert near = (far==FAR_0)?NEAR_0:NEAR_1;
  const Vector2 dir = (verts[near]-lightSource).getNormalized();
  verts[far] = verts[near]+dir*FARDIST;
}

void ShadowPolygon::_calculatePenumbra (const Vector2& lightSource,
                                            const Vector2& bboxPos,
                                            eVert penum) {
  const eVert near = (penum == PENUMBRA_0) ? NEAR_0 : NEAR_1;
  const Vector2 cv = (verts[near] - bboxPos).getNormalized();
  const Vector2 tmp = (verts[near] - lightSource).getNormalized();
  Vector2 p = Vector2(tmp.y, -tmp.x);
  if (cv * p < 0)
    p = -p;

  const Vector2 dir = (verts[near] + p * 0.2f - lightSource).getNormalized();
  verts[penum] = verts[near] + dir * FARDIST;

  //To calculate the inner edge (FAR_0) as in the article
  // http://www.gamedev.net/reference/programming/features/2dsoftshadow/page4.asp
  // we should use that. But just using C as the inner edge works fine and save some calculation
  /*const Vector2 dir2 = (verts[NEAR_0]-p*0.2f-lightSource).getNormalized();
  verts[FAR_0] = verts[NEAR_0]+dir2*FARDIST;*/
}

ShadowPolygon::ShadowPolygon (const Vector2& lightSource,
                                  const AABBox* bbox,
                                  const Vector2& bboxPos) {
  _castShadow(lightSource, bbox, bboxPos);
  _calculatePenumbra(lightSource, bboxPos, PENUMBRA_0);
  _calculatePenumbra(lightSource, bboxPos, PENUMBRA_1);
  _calculateFar(lightSource, FAR_0);
  _calculateFar(lightSource, FAR_1);

  lines[0] = new Line(verts[NEAR_0], verts[PENUMBRA_0]);
  lines[1] = new Line(verts[PENUMBRA_0], verts[PENUMBRA_1]);
  lines[2] = new Line(verts[PENUMBRA_1], verts[NEAR_1]);
  lines[3] = new Line(verts[NEAR_1], verts[NEAR_0]);

  //Make sure all line normals are pointing outwards the volume
  //To check that, we construct the vector going from PENUMBRA_1 to the middle of (PENUMBRA_0,NEAR_0). This
  //vector should have the same direction (angle between them > 0 and < M_PI) as the line normal.
  //If this isn't the case, flip all normals
  const Vector2 ctrl = ((verts[PENUMBRA_0] + verts[NEAR_0])/2.0f - verts[PENUMBRA_1]).getNormalized();
  float angle = ctrl*lines[0]->getNormal();
  if (angle < 0 || angle > M_PI) {
    for (int i=0; i<4; i++)
      lines[i]->flipNormal();
  }
}
