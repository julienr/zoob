#ifndef CURSORVIEW_H_
#define CURSORVIEW_H_

#include "view/Sprite.h"
#include "logic/Cursor.h"

class CursorView {
  public:
    CursorView (const Cursor& c)
      : cursor(c), sprite("assets/sprites/cursor.png", TEX_GROUP_GAME) {}

    Vector2 getCenter () {
      return sprite.getCenter(cursor);
    }

    void draw () {
      sprite.draw(cursor);
    }
  private:
    const Cursor& cursor;
    Sprite sprite;

};

#endif /* CURSORVIEW_H_ */
