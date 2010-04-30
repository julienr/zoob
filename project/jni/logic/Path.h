#ifndef PATH_H_
#define PATH_H_

//A path on the level that a tank can follow
struct Path {
  Path (size_t numNodes, Vector2* wp)
    : numNodes(numNodes), waypoints(wp) {}

  ~Path () {
    delete [] waypoints;
  }

  size_t numNodes;
  //contains the position on the path
  Vector2* waypoints;
};

#endif /* PATH_H_ */
