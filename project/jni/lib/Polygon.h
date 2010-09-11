#ifndef POLYGON_H_
#define POLYGON_H_

#include "lib/Line.h"

enum eRelativePos {
    INSIDE = 0,
    OUTSIDE,
    INTERSECT
};

class Polygon {
  public:
    //Test if a point is inside this volume
    virtual bool inside (const Vector2& p) const = 0;

    //Test if a circle is FULLY inside this polygon
    virtual eRelativePos classifyCircle (const Vector2& center, float r) const = 0;

    virtual size_t getNumVerts () const = 0;
    virtual const Vector2* getVerts () const = 0;

    virtual size_t getNumSides () const = 0;
    virtual const Line* const* getSides () const = 0;
};

#endif /* POLYGON_H_ */
