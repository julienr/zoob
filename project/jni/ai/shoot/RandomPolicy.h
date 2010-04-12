#ifndef RANDOMPOLICY_H_
#define RANDOMPOLICY_H_

#include "ShootPolicy.h"

#define FIRE_INTERVAL 1.0f
#define FIRE_ROTATION_SPEED 0.5f

class RandomPolicy : public ShootPolicy {
  public:
    RandomPolicy () : timeSinceFire(0), angle(0) {}

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

    Vector2 aim (double elapsedS, Game* game, Tank* tank);
  private:
    double timeSinceFire;
    float angle;
};

#endif /* RANDOMPOLICY_H_ */
