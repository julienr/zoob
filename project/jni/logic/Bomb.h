#ifndef BOMB_H_
#define BOMB_H_

#include "ExplosiveEntity.h"
#include "Tank.h"

//TODO: Let the mines have cooldown before explosion => A mine exploded when you pass on it OR after its delay
class Bomb : public ExplosiveEntity {
  public:
    Bomb (Tank* owner, const Vector2& pos)
      : ExplosiveEntity(new BCircle(BOMB_BCIRCLE_R)),
        owner(owner),
        timeLeft(BOMB_LIFETIME) {
      setPosition(pos);
    }

    eEntityType getType () const {
      return ENTITY_MINE;
    }
    
    Tank* getOwner () {
      return owner;
    }
    
    void think (double elapsedS) {
      timeLeft -= elapsedS;
    }
    
    double getTimeLeft () const {
      return timeLeft;
    }
  private:
    Tank* owner;
    double timeLeft;
};

#endif /* BOMB_H_ */
