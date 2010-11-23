#ifndef NETTANK_H_
#define NETTANK_H_

#include "Tank.h"
#include "Game.h"

class NetTank: public Tank {
  public:
    NetTank (FireRatePolicy* pol) : Tank(TANK_BCIRCLE_R, pol) {}
      
    eTankType getTankType () const {
      return TANK_PLAYER;
    }

    eTankCategory getTankCategory () const {
      return CAT_NET; 
    }

  protected:
    Rocket* createRocket (Tank* owner, const Vector2& pos, const Vector2& dir);
};

#endif
