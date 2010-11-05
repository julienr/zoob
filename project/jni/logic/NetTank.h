#ifndef NETTANK_H_
#define NETTANK_H_

#include "Tank.h"

class NetTank: public Tank {
  public:
    NetTank () : Tank(TANK_BCIRCLE_R, new DummyFirePolicy()) {}
      
    eTankType getTankType () const {
      return TANK_PLAYER;
    }

    eTankCategory getTankCategory () const {
      return CAT_NET; 
    }

  protected:
    Rocket* createRocket (Tank* owner, const Vector2& pos, const Vector2& dir) {
      //Shouldn't be called
      LOGE("[NetTank::createRocket] shouldn't be called");
      return NULL;
    }
};

#endif
