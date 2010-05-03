#ifndef PLAYERTANK_H_
#define PLAYERTANK_H_

#include "Tank.h"

class PlayerTank : public Tank {
  public:
    PlayerTank ()
      : Tank(new IntervalFirePolicy(1000)) {
    }

    eTankType getTankType () const { return TANK_PLAYER; }

};

#endif /* PLAYERTANK_H_ */
