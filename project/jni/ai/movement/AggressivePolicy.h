#ifndef AGGRESSIVEPOLICY_H_
#define AGGRESSIVEPOLICY_H_

#include "MovementPolicy.h"

/**
 * This is a pathfinding-driven policy. The tank will
 * 1) dodge rockets
 * 2) try to escape from the player by always going as far away from him as possible
 */
class AggressivePolicy : public MovementPolicy {
  public:
    bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);
};

#endif /* AGRESSIVEPOLICY_H_ */
