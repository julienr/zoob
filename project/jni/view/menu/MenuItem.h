#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "lib/Utils.h"
#include "def.h"
#include "view/Sprite.h"
#include "view/Color.h"

class MenuItem {
  public:
    MenuItem(const char* tex, const char* hoverTex, short id, int groupID, bool repeatable=false) :
      id(id),
      normalSprite(tex, groupID),
      hoverSprite(hoverTex, groupID),
      pressed(false),
      repeatable(repeatable) {}

    short getID () const {
      return id;
    }

    void draw () const;
    void drawPressed () const;

    void setPosition (const Vector2& pos) {
      this->pos = pos;
    }

    const Vector2& getPosition () {
      return pos;
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

    //FIXME: used ONLY by input manager, also use for menus
    void setPressed (bool p) {
      pressed = p;
    }

    bool isPressed () const {
      return pressed;
    }

    bool isRepeatable () const {
      return repeatable;
    }
  private:
    short id;
    Vector2 pos;
    Vector2 size;
    Vector2 bbPos;
    Vector2 bbSize;
    Sprite normalSprite;
    Sprite hoverSprite;

    bool pressed;
    bool repeatable;
};

#endif /* MENUITEM_H_ */
