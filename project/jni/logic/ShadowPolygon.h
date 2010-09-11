#ifndef SHADOWPOLYGON_H_
#define SHADOWPOLYGON_H_

#include "def.h"
#include "logic/physics/AABBox.h"
#include "lib/Line.h"
#include "lib/Polygon.h"

class Tile;
class Level;

/**
 * This represent the shadow "volume" casted by a 2d-polygon.
 * A shadow polygon consist of 4 points. two are the "near" points (from which shadow is casted)
 * and two are the far points (the end of the polygon).
 * In zoob, a shadow polygon always extends to the whole level.
 */
class ShadowPolygon : public Polygon {
  public:
    /** Create a shadow polygon for the shadow casted by lightSource and the given aabbox
     * */
    ShadowPolygon (const Vector2& lightSource,
                     const AABBox* bbox,
                     const Vector2& bboxPos);

    ~ShadowPolygon () {
      for (int i=0; i<4; i++)
        delete lines[i];
    }

    enum eVert {
        NEAR_0 = 0,
        NEAR_1 = 1,
        FAR_0 = 2,
        FAR_1 = 3,
        PENUMBRA_0=4,
        PENUMBRA_1=5,
        NUM_VERTS=6
    };
    const Vector2& getVert (eVert which) const { return verts[which]; }

    //Test if a point is inside this volume
    bool inside (const Vector2& p) const;

    //Test if a bbox is inside this volume
    bool inside (const Vector2& pos, const AABBox& bbox) const;

    //Test if a circle is FULLY inside this polygon
    eRelativePos classifyCircle (const Vector2& center, float r) const;

    size_t getNumVerts () const { return 6; }
    //The vertices, returned in an order suitable for drawing (NEAR_0,FAR_0,FAR_1,NEAR_1)
    const Vector2* getVerts () const { return verts; }

    size_t getNumSides () const { return 4; }
    const Line* const* getSides () const { return lines; }

  private:
    void _castShadow (const Vector2& lightSource,
                        const AABBox* bbox,
                        const Vector2& bboxPos);
    void _calculateFar (const Vector2& lightSource,
                          eVert far);

    void _calculatePenumbra (const Vector2& lightSource,
                                const Vector2& bboxPos,
                                eVert penum);
    Vector2 verts[6];
    Line* lines[4]; //the four lines that delimit this shadow polygon
};

#endif /* SHADOWPOLYGON_H_ */
