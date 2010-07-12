#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "lib/Utils.h"
#include "def.h"
#include "view/Sprite.h"
#include "view/Color.h"

class MenuItem {
  public:
    MenuItem(short id, bool repeatable) :
      id(id),
      repeatable(repeatable),
      pressed(false) {}

    short getID () const {
      return id;
    }

    virtual void draw () const = 0;

    void setPosition (const Vector2& pos) {
      this->pos = pos;
    }

    const Vector2& getPosition () const {
      return pos;
    }

    void setSize (const Vector2& size) {
      this->size = size;
    }

    const Vector2& getSize () const {
      return size;
    }

    //bounding box
    void setBB (const Vector2& pos, const Vector2& size) {
      bbPos = pos;
      bbSize = size;
    }

    virtual bool inside (const Vector2& p) const {
      return Utils::insideC(bbPos, bbSize, p);
    }

    //FIXME: used ONLY by input manager, also use for menus
    void setPressed (bool p) {
      pressed = p;
    }

    bool isRepeatable () const {
      return repeatable;
    }

    bool isPressed () const {
      return pressed;
    }

  protected:
    void drawBB (const Vector2& pos, const Vector2& size);

  private:
    short id;
    Vector2 pos;
    Vector2 size;
    Vector2 bbPos;
    Vector2 bbSize;

    bool repeatable;
    bool pressed;
};

#endif /* MENUITEM_H_ */
