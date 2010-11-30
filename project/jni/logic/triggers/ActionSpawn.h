#ifndef ACTION_SPAWN_H_
#define ACTION_SPAWN_H_

class ActionSpawn : public Action {
  public:
    ActionSpawn (int dx, int dy) 
      : dx(dx), dy(dy) {}

    void execute (const Tile* /*tile*/) const {
      LOGE("spawning enemy at (%i,%i)", dx, dy);
    }
  private:
    const int dx;
    const int dy;
};

#endif
