#ifndef PATHPOLICY_H_
#define PATHPOLICY_H

#include "def.h"
#include "lib/Vector2.h"
#include "containers/vector.h"
#include "MovementPolicy.h"


class PathPolicy : public MovementPolicy {
  public:
    PathPolicy ()
      : prev(0) {}

    bool decideDir (double elapsedS, Vector2* outDir, Game* game, Tank* tank);

  private:
    size_t prev; //idx of last vertex of the path we passed on
};

#endif /* PATHPOLICY_H_ */
