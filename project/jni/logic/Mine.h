#ifndef MINE_H_
#define MINE_H_

#include "ExplosiveEntity.h"
#include "Tank.h"

//TODO: Let the mines have cooldown before explosion => A mine exploded when you pass on it OR after its delay
class Mine : public ExplosiveEntity {
  public:
    Mine (Tank* owner, const Vector2& pos)
      : ExplosiveEntity(new BCircle(MINE_BCIRCLE_R)),
        owner(owner),
        timeLeft(MINE_LIFETIME) {
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
    
    virtual void explode (Entity* e, const Vector2& colPoint) {
      ExplosiveEntity::explode(e, colPoint);
      owner->mineExploded();
    }
  private:
    Tank* owner;
    double timeLeft;
};

#endif /* MINE_H_ */
