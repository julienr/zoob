#ifndef ENEMYTANKVIEW_H_
#define ENEMYTANKVIEW_H_

#include "TankView.h"
#include "logic/EnemyTank.h"

//When a tank is preparing to fire, it will grow from 0.7*size to 1.5*size
#define TANK_START_FIRING_SIZE 0.7f
#define TANK_END_FIRING_SIZE 1.2f

class EnemyTankView : public TankView {
  public:
    EnemyTankView (const EnemyTank& t)
      : TankView(t),
        eTank(&t) {}

    float getTankScale () {
      if (eTank->isFiring())
        return TANK_END_FIRING_SIZE; //burst fire => display at big size
      if (!eTank->isPreparingFire())
        return 1.0f;
      const float maxDelay = Difficulty::getInstance()->getFiringDelay();
      const float sizeIncr = (TANK_END_FIRING_SIZE-TANK_START_FIRING_SIZE)/maxDelay;
      return TANK_START_FIRING_SIZE + sizeIncr*(maxDelay - eTank->getFiringDelay());
    }
  private:
    const EnemyTank* eTank;
};

#endif /* ENEMYTANKVIEW_H_ */
