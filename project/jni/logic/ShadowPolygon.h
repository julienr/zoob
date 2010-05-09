#ifndef SHADOWPOLYGON_H_
#define SHADOWPOLYGON_H_

#include "def.h"
#include "logic/physics/AABBox.h"

class Tile;
class Level;

/**
 * This represent the shadow "volume" casted by a 2d-polygon.
 * A shadow polygon consist of 4 points. two are the "near" points (from which shadow is casted)
 * and two are the far points (the end of the polygon).
 * In zoob, a shadow polygon always extends to the whole level.
 */
class ShadowPolygon {
  public:
    /** Create a shadow polygon for the shadow casted by lightSource and the given aabbox
     * */
    ShadowPolygon (const Vector2& lightSource,
                     const AABBox* bbox,
                     const Vector2& bboxPos);

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

    //The vertices, returned in an order suitable for drawing (NEAR_0,FAR_0,FAR_1,NEAR_1)
    const Vector2* getVerts () const { return verts; }

  private:
    void _castShadow (const Vector2& lightSource,
                        const AABBox* bbox,
                        const Vector2& bboxPos);
    void _calculateFar (const Vector2& lightSource,
                          eVert far);
    Vector2 verts[6];
};

#endif /* SHADOWPOLYGON_H_ */
