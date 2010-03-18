#ifndef LEVELVIEW_H_
#define LEVELVIEW_H_

#include "TextureManager.h"
#include "lib/Utils.h"
#include "logic/Level.h"
#include "Sprite.h"

class LevelView {
  public:
    LevelView (const Level* lvl)
      : level(lvl),
        emptySprite("assets/sprites/earth.png"),
        wallSprite("assets/sprites/crate.png"),
        spawnSprite("assets/sprites/earth.png") {}

    void draw ();

    Vector2 getCenter () {
      return Vector2(level->getWidth()/2.0f, level->getHeight()/2.0f);
    }

    bool inside (const Vector2& p) {
      const Vector2 extents(level->getWidth(), level->getHeight());
      return Utils::inside(Vector2(0,0), extents.x, extents.y, p);
    }
  private:
    const Sprite emptySprite;
    const Sprite wallSprite;
    const Sprite spawnSprite;
    const Level* level;
};

#endif /* LEVELVIEW_H_ */
