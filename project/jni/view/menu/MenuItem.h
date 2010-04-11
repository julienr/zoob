#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "lib/Utils.h"
#include "def.h"
#include "view/Sprite.h"

class MenuItem {
  public:
    MenuItem(const char* tex, const char* hoverTex, short id) :
      id(id),
      normalSprite(tex),
      hoverSprite(hoverTex) {}

    short getID () const {
      return id;
    }

    void draw () const;

    void drawHover () const;

    void setPosition (const Vector2& pos) {
      this->pos = pos;
    }

    void setSize (const Vector2& size) {
      this->size = size;
    }

    //bounding box
    void setBB (const Vector2& pos, const Vector2& size) {
      bbPos = pos;
      bbSize = size;
    }

    bool inside (const Vector2& p) const {
      return Utils::insideC(bbPos, bbSize, p);
    }

  private:
    short id;
    Vector2 pos;
    Vector2 size;
    Vector2 bbPos;
    Vector2 bbSize;
    Sprite normalSprite;
    Sprite hoverSprite;
};

#endif /* MENUITEM_H_ */
