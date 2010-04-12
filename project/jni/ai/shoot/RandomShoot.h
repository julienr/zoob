#ifndef RANDOMSHOOT_H_
#define RANDOMSHOOT_H_

#include "ShootPolicy.h"

#define FIRE_INTERVAL 1.0f
#define FIRE_ROTATION_SPEED 0.5f

class RandomShoot : public ShootPolicy {
  public:
    RandomShoot () : timeSinceFire(0), angle(0) {}

    bool decideFire (double elapsedS, Vector2* outDir) {
      timeSinceFire += elapsedS;
      angle += elapsedS*FIRE_ROTATION_SPEED;
      if (timeSinceFire > FIRE_INTERVAL) {
        timeSinceFire = 0;
        outDir->set(cosf(angle), sinf(angle));
        return true;
      }
      return false;
    }
  private:
    double timeSinceFire;
    float angle;
};

#endif /* RANDOMSHOOT_H_ */
