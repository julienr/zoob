#include "MenuItem.h"
#include "view/GLW.h"
#include "view/Square.h"

//draw bounding box around objects
void drawBB (const Vector2& pos, const Vector2& size) {
  glColor4f(1,0,0,0.7f);
  glPushMatrix();
  glDisable(GL_TEXTURE_2D);
  GLW::translate(pos.x, pos.y, 0);
  GLW::scale(size.x, size.y, 1);
  Square::draw(false);
  glPopMatrix();
  glEnable(GL_TEXTURE_2D);
  glColor4f(1,1,1,1);
}

void MenuItem::draw () const {
  if (pressed)
    hoverSprite.draw(pos, size);
  else
    normalSprite.draw(pos, size);
  drawBB(bbPos, bbSize);
}

void MenuItem::drawPressed () const {
  hoverSprite.draw(pos, size);
}
