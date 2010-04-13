#ifndef PATHPOLICY_H_
#define PATHPOLICY_H

#include "def.h"
#include "lib/Vector2.h"
#include "containers/vector.h"
#include "MovementPolicy.h"


class PathPolicy : public MovementPolicy {
  public:
    PathPolicy (const vector<Vector2>& path)
      : path(path) {
    }

    Vector2 decideDir (double elapsedS) {

    }


  private:
    size_t prev; //idx of last vertex of the path we passed on
    vector<Vector2> path;
};

#endif /* PATHPOLICY_H_ */
