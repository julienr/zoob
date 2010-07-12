#ifndef CHECKBOX_H_
#define CHECKBOX_H_

#include "MenuItem.h"
#include "view/Sprite.h"

class Checkbox : public MenuItem {
  public:
    enum eState {
      CHECKED=0,
      UNCHECKED=1
    };

    Checkbox (short id, int groupID, bool checked)
      : MenuItem(id, false),
        pressed(false),
        spriteUnchecked("assets/sprites/menuitems/checkbox_unchecked.png", groupID),
        spriteUnchecked_h("assets/sprites/menuitems/checkbox_unchecked_h.png", groupID),
        spriteChecked("assets/sprites/menuitems/checkbox_checked.png", groupID),
        spriteChecked_h("assets/sprites/menuitems/checkbox_checked_h.png", groupID),
        state(UNCHECKED) {}

    short getID () const {
      return id;
    }

    void draw () const;

    eState getState () const {
      return state;
    }

    void toggle () {
      state = (state==CHECKED)?UNCHECKED:CHECKED;
    }

    void setChecked (bool c) {
      state = c?CHECKED:UNCHECKED;
    }

    bool isChecked () {
      return state == CHECKED;
    }

  private:
    short id;

    bool pressed;

    Sprite spriteUnchecked;
    Sprite spriteUnchecked_h;
    Sprite spriteChecked;
    Sprite spriteChecked_h;

    eState state;
};

#endif /* CHECKBOX_H_ */
