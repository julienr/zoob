#include "Polygon.h"
#include "containers/vector.h"

Polygon::Polygon (const vector<Vector2>& verts)
  : numVerts(verts.length()) {
  this->verts = new Vector2[numVerts];
  for (size_t i=0; i<numVerts; i++)
    this->verts[i] = verts[i];
  _calculateSides();
}

Polygon::Polygon (const Vector2* verts, size_t numVerts)
  : numVerts(numVerts) {
  this->verts = new Vector2[numVerts];
  for (size_t i=0; i<numVerts; i++)
    this->verts[i] = verts[i];
  _calculateSides();
}

Polygon::Polygon (const Polygon& other)
  : numVerts(other.numVerts) {
  this->verts = new Vector2[numVerts];
  for (size_t i=0; i<numVerts; i++)
      this->verts[i] = other.verts[i];
  _calculateSides();
}

Polygon::~Polygon () {
  delete [] verts;
}

void Polygon::_calculateSides () {
  this->sides = new Line[numVerts];
  for (size_t i=0; i<numVerts; i++) {
    sides[i] = Line::fromPoints(verts[i], verts[(i+1)%numVerts]);
  }
}

//Return the intersection between the line going from p1 to p2 and given 'line'
//ASSUME an intersection exists, result is random if no intersection exists
static Vector2 intersect (const Vector2& p1, const Vector2& p2, const Line& line) {
  Line l2 = Line::fromPoints(p1,p2);
  Vector2 result;
  eLineIntersect r = line.intersect(l2, &result);
//  LOGE("l1[(%f,%f),%f], l2[(%f,%f),%f] => intersect(%i) at (%f,%f)", line.getNormal().x, line.getNormal().y, line.getC(), l2.getNormal().x, l2.getNormal().y, l2.getC(), r==INTERSECT_ONCE, result.x, result.y);
  ASSERT(r == INTERSECT_ONCE);
  return result;
}

Polygon Polygon::clip(Line* clipLines, int numClipLines) {
  //LOGE("clipping with %i clip lines", numClipLines);
  //LOGE("clip : initial numverts : %i", getNumVerts());
  vector<Vector2> poly(getNumVerts(), getVerts());
  vector<Vector2> clippedPoly(poly.size());

  for (int i=0; i<numClipLines; i++) {
    const Line& l = clipLines[i];
    clippedPoly.clear();

    const size_t size = poly.size();
    for (size_t j=0; j<size; j++) {
      const Vector2& p = poly[j];
      const Vector2& p_next = poly[(j+1)%size];
      eLineRelPos curPos = l.getRelativePosition(p);
      eLineRelPos nextPos = l.getRelativePosition(p_next);

      if (curPos == BEHIND) { //current point inside clipping region
       if (nextPos == BEHIND)  //next is also inside => no clipping needed
         clippedPoly.append(p_next);
       else //clipping needed
         clippedPoly.append(intersect(p, p_next, l));
      } else { //current point oustide clipping region
        if (nextPos == BEHIND) { //but next is inside, so clip
          clippedPoly.append(intersect(p, p_next, l));
          clippedPoly.append(p_next);
        } //else both points outside
      }
    }
    //replace poly with clipped polygon
    poly.clear();
    clippedPoly.appendTo(poly);
  }

  //LOGE("clip : clipped numverts : %i", poly.size());
  return Polygon(poly);
}
