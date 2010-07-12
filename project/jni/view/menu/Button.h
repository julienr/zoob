#ifndef BUTTON_H_
#define BUTTON_H_

#include "MenuItem.h"

class Button : public MenuItem {
  public:
    Button (const char* tex, const char* hoverTex, short id, int groupID, bool repeatable=false)
      : MenuItem(id, repeatable),
        normalSprite(tex, groupID),
        hoverSprite(hoverTex, groupID),
        enabled(true) {}

    void draw () const;
    void drawPressed () const;

    bool inside (const Vector2& p) const {
      return enabled && MenuItem::inside(p);
    }

    //When disabled, inside() will always return false and as a result, the button won't be activeable
    void setEnabled (bool b) {
      enabled = b;
    }

    bool isEnabled () const {
      return enabled;
    }
  private:
    Sprite normalSprite;
    Sprite hoverSprite;

    bool enabled;
};

#endif /* BUTTON_H_ */
