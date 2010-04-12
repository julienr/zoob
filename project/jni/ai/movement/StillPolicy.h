#ifndef STILLPOLICY_H_
#define STILLPOLICY_H_

#include "MovementPolicy.h"

class StillPolicy : public MovementPolicy {
  public:

    Vector2 decideDir (double elapsedS) {
      return Vector2(0,0);
    }
};

#endif /* STILLPOLICY_H_ */
