#ifndef PATH_H_
#define PATH_H_

#include "containers/vector.h"

//A path on the level that a tank can follow
class Path {
  public:
    Path (size_t numNodes, Vector2* wp)
      : numNodes(numNodes), waypoints(wp) {}

    ~Path () {
      delete [] waypoints;
    }

    size_t length () {
      return numNodes;
    }

    const Vector2& get(size_t i) {
      return waypoints[i];
    }
  private:
    size_t numNodes;
    Vector2* waypoints;
};

#endif /* PATH_H_ */
