#ifndef PATHPOLICY_H_
#define PATHPOLICY_H_

#include "def.h"
#include "lib/Vector2.h"
#include "containers/vector.h"
#include "MovementPolicy.h"

class PathPolicy : public MovementPolicy {
  public:
    PathPolicy ()
      : current(0) {}

    bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);

  private:
    size_t current; //idx the current waypoint we're trying to reach
};

#endif /* PATHPOLICY_H_ */
