#ifndef _EXPLOSION_INFO_H
#define	_EXPLOSION_INFO_H

//Describe an explosion
struct ExplosionInfo {
    enum eType {
        EXPLOSION_BOOM,
        EXPLOSION_POOF
    };
    ExplosionInfo (const Vector2& p, eType t)
      : position(p), type(t) {}
    Vector2 position;
    eType type;
    //Contains informations (Entity* entity, int damage) for each entity
    //damaged by the explosion
    list<pair<Entity*, int> > damagedEntities;
};


#endif	/* _EXPLOSION_INFO_H */

