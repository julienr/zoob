#ifndef RANDOMPOLICY_H_
#define RANDOMPOLICY_H_

#include "ShootPolicy.h"

#define FIRE_INTERVAL 1.0f
#define FIRE_ROTATION_SPEED 0.5f

class RandomPolicy : public ShootPolicy {
  public:
    RandomPolicy () : timeSinceFire(0), angle(0) {}


    bool decideFire (double elapsedS, Vector2* outDir, Game* game, Tank* tank);
    bool aim (double elapsedS, Game* game, Tank* tank, Vector2* outDir);
  private:
    double timeSinceFire;
    float angle;
};

#endif /* RANDOMPOLICY_H_ */
