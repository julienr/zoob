#ifndef _EXPLOSIONLOCATION_H
#define	_EXPLOSIONLOCATION_H

//Describe an explosion
struct ExplosionLocation {
    enum eType {
        EXPLOSION_BOOM,
        EXPLOSION_POOF
    };
    ExplosionLocation (const Vector2& p, eType t)
      : position(p), type(t) {}
    Vector2 position;
    eType type;
};


#endif	/* _EXPLOSIONLOCATION_H */

