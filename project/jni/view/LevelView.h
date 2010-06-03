#ifndef LEVELVIEW_H_
#define LEVELVIEW_H_

#include "lib/Utils.h"
#include "logic/Level.h"
#include "Sprite.h"

class LevelView {
  public:
    LevelView (const Level* lvl)
      : level(lvl),
        wallSprite("assets/sprites/crate.png", TEX_GROUP_GAME),
        spawnSprite("assets/sprites/earth.png", TEX_GROUP_GAME){}

    void drawWalls ();
    void drawBackground ();

    Vector2 getCenter () {
      return Vector2(level->getWidth()/2.0f, level->getHeight()/2.0f);
    }

    bool inside (const Vector2& p) {
      const Vector2 extents(level->getWidth(), level->getHeight());
      return Utils::insideTL(Vector2(0,0), extents.x, extents.y, p);
    }

  private:
    const Level* level;
    const Sprite wallSprite;
    const Sprite spawnSprite;
};

#endif /* LEVELVIEW_H_ */
