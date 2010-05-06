#ifndef MOVEMENTPOLICY_H_
#define MOVEMENTPOLICY_H_

class Game;
class EnemyTank;

class MovementPolicy {
  public:
    virtual bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) = 0;
};

#endif /* MOVEMENTPOLICY_H_ */
