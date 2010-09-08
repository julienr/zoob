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

    Path* copy () const {
      Vector2* wp = new Vector2[numNodes];
      for (size_t i=0; i<numNodes; i++)
        wp[i] = waypoints[i];
      return new Path(numNodes, wp);
    }

    size_t length () const {
      return numNodes;
    }

    const Vector2& get(size_t i) const {
      return waypoints[i];
    }
  private:
    //DISALLOW copy
    Path (const Path& UNUSED(other)) {}

    size_t numNodes;
    Vector2* waypoints;
};

#endif /* PATH_H_ */
