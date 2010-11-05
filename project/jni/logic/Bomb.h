#ifndef BOMB_H_
#define BOMB_H_

#include "ExplosiveEntity.h"
#include "Tank.h"

//A mine explodes when an enemy pass on it OR after its delay
class Bomb : public ExplosiveEntity {
  public:
    Bomb (Tank* owner, const Vector2& pos)
      : ExplosiveEntity(new BCircle(BOMB_BCIRCLE_R)),
        owner(owner),
        timeLeft(BOMB_LIFETIME) {
      setPosition(pos);
    }

    eEntityType getType () const {
      return ENTITY_BOMB;
    }
    
    Tank* getOwner () {
      return owner;
    }

    void think (double elapsedS) {
      timeLeft -= elapsedS;
    }

    void setTimeLeft (double t) {
      timeLeft = t;
    }

    double getTimeLeft () const {
      return timeLeft;
    }
  private:
    Tank* owner;
    double timeLeft;
};

#endif /* BOMB_H_ */
