#ifndef TANKAI_H_
#define TANKAI_H_

#include "lib/Vector2.h"

/**
 * Base abstract class for a tank AI
 * all the "decideXX" functions will be called
 * every frame
 */
class TankAI {
  public:
    virtual bool decideFire (double elapsedS, Vector2* outDir) = 0;
    virtual Vector2 decideDir (double elapsedS) = 0;
};

#endif
