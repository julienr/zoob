#ifndef SMARTPOLICY_H_
#define SMARTPOLICY_H_

#include "lib/Vector2.h"
#include "logic/Path.h"
#include "ai/movement/MovementPolicy.h"

class Game;
class EnemyTank;

class SmartPolicy : public MovementPolicy {
  public:
    SmartPolicy () : MovementPolicy(), mode(DEFENSIVE), modeElapsedS(0), shortestWay(NULL) {}

    bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* me);

    bool requirePlayerVisibility () { return true; }

  private:
    //This function will try to slightly adjust our position to correct for the situation where we have a LINE
    //of sight on the player but since rockets have a circle RADIUS, we cannot fire because they would reach an obstacle
    //This ONLY work if the tank (me) is on a VISIBLE cell
    //returns true if a move is needed, false otherwise
    bool adjustForFiring (Vector2* outDir, Game* game, EnemyTank* me);

    void planPath (Game* game, EnemyTank* me);

    enum eMode {
      AGGRESSIVE=0,
      DEFENSIVE
    };

    void switchMode (eMode m) {
      mode = m;
      modeElapsedS = 0;
    }

    eMode mode;
    double modeElapsedS;

    //the path currently being followed
    Path* shortestWay;
};

#endif /* SMARTPOLICY_H_ */
