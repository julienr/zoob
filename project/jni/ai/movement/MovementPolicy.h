#ifndef MOVEMENTPOLICY_H_
#define MOVEMENTPOLICY_H_

class Game;
class Tank;

class MovementPolicy {
  public:
    virtual Vector2 decideDir (double elapsedS) = 0;
};

#endif /* MOVEMENTPOLICY_H_ */
