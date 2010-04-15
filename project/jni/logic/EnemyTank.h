#ifndef ENEMYTANK_H_
#define ENEMYTANK_H_

#include "Tank.h"
#include "ai/TankAI.h"

class EnemyTank: public Tank {
  public:
    EnemyTank(TankAI* ai)
      : Tank(2000),
        ai(ai){
    }

    virtual ~EnemyTank () {
      delete ai;
    }

    //Returns NULL for the player's tank
    TankAI* getAI () { return ai; }

  private:
    TankAI* ai;
};

#endif /* ENEMYTANK_H_ */
