#ifndef PATHPOLICY_H_
#define PATHPOLICY_H_

#include "def.h"
#include "lib/Vector2.h"
#include "containers/vector.h"
#include "MovementPolicy.h"
#include "lib/Utils.h"

class PathPolicy : public MovementPolicy {
  public:
    PathPolicy ()
      : current(0), prevSec(Utils::getCurrentTimeMillis()) {}

    bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);

  private:
    size_t current; //idx the current waypoint we're trying to reach
    /* This is used to avoid tanks getting stuck.
     * At fixed time interval, we record the distance we have covered. If this distance is pretty small,
     * this means we are making no progress towards our target waypoint. This will happen when we are
     * in a deadlock with other tanks all trying to go to the same waypoint (pathfinding isn't really
     * helpfull here because we already are at the good cell, just not close enough to the waypoint)
     */
    uint64_t prevSec;
    Vector2 prevSecPos;
};

#endif /* PATHPOLICY_H_ */
