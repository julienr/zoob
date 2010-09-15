#ifndef POLYGON_H_
#define POLYGON_H_

#include "containers/vector.h"
#include "lib/Line.h"

enum eRelativePos {
    INSIDE = 0,
    OUTSIDE,
    INTERSECT
};

class Polygon {
  public:
    Polygon (const vector<Vector2>& verts);
    //create a polygon by COPYING the vertices in verts
    Polygon (const Vector2* verts, size_t numVerts);
    Polygon (const Polygon& other);
    ~Polygon ();

    size_t getNumVerts () const { return numVerts; }
    const Vector2* getVerts () const { return verts; }

    size_t getNumSides () const { return numVerts; }
    const Line* getSides () { return sides; }


    //clip the polygon. The clipping region is delimited by clipLines. What lies IN_FRONT of this lines will
    //be clipped (normals should be facing OUTWARD the clipping region)
    //Returns a newly allocated polygon
    Polygon clip(Line* clipLines, int numClipLines);

  protected:
    void _calculateSides ();
    const size_t numVerts;
    Vector2* verts;
    Line* sides;

};

#endif /* POLYGON_H_ */
