#ifndef SMARTPOLICY_H_
#define SMARTPOLICY_H_

#include "lib/Vector2.h"
#include "logic/Path.h"
#include "ai/movement/MovementPolicy.h"

class Game;
class EnemyTank;

class SmartPolicy : public MovementPolicy {
  public:
    SmartPolicy () : MovementPolicy(), mode(DEFENSIVE), modeElapsedS(0) {}

    bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);

    bool requirePlayerVisibility () { return true; }

  private:
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
};

#endif /* SMARTPOLICY_H_ */
