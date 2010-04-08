#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "lib/Utils.h"
#include "def.h"
#include "view/Sprite.h"

class MenuItem {
  public:
    MenuItem(const char* tex, const char* hoverTex) :
      normalSprite(tex),
      hoverSprite(hoverTex) {}

    void draw () const {
      normalSprite.draw(pos, size);
    }

    void drawHover () const {
      hoverSprite.draw(pos, size);
    }

    void setPosition (const Vector2& pos) {
      this->pos = pos;
    }

    void setSize (const Vector2& size) {
      this->size = size;
    }

    bool inside (const Vector2& p) const {
      return Utils::insideC(pos, size, p);
    }

  private:
    Vector2 pos;
    Vector2 size;
    Sprite normalSprite;
    Sprite hoverSprite;
};

#endif /* MENUITEM_H_ */
