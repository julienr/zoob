#ifndef GUIDEDROCKET_H_
#define GUIDEDROCKET_H_

#include "Rocket.h"

class GuidedRocket : public Rocket {
  public:
    GuidedRocket (Tank* owner, const Vector2& pos, const Vector2& dir, Tank* target)
      : Rocket(owner, pos, dir),
        target(target) {
    }

    bool canBounce () const {
      return false;
    }

    void think (double elapsedS);
  private:
    const Tank* target;
};

#endif /* GUIDEDROCKET_H_ */
