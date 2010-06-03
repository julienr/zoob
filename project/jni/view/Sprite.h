#ifndef SPRITE_H_
#define SPRITE_H_

#include "textures/TextureManager.h"
#include "logic/Viewable.h"
#include "lib/Utils.h"

class Sprite {
  public:
    Sprite (const char* filename, int groupID) {
      texture = TextureManager::getInstance()->getGroup(groupID)->get(filename);
    }

    void draw (const Viewable& viewable, float rotation=0, float scale=1) const;
    void draw (const Vector2& pos, const Vector2& size, float rotation=0) const;

    Vector2 getCenter (const Viewable& viewable) const {
      return Vector2(viewable.getPosition().x+viewable.getWidth()/2.0f,
                      viewable.getPosition().y+viewable.getHeight()/2.0f);
    }

    //Since this is for touch use, double the area
    bool touchInside (const Viewable& viewable, const Vector2& p) const {
      const Vector2 ext = viewable.getSize()*2;
      return Utils::insideTL(viewable.getPosition()-(ext/2), ext.x, ext.y, p);
    }

    inline
    void bind () const {
      texture->bind();
    }
  private:
    Texture* texture;
};

#endif /* SPRITE_H_ */
