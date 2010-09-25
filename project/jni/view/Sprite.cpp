#include "Sprite.h"
#include "view/GLW.h"
#include "view/primitives/Square.h"

void Sprite::draw (const Viewable& viewable, float rotation, float scale) const {
  bind();
  glPushMatrix();
  GLW::translate(viewable.getPosition().x, viewable.getPosition().y, 0);
  GLW::rotate(RAD_TO_DEG(rotation), 0, 0, 1);
  GLW::scale(viewable.getWidth()*scale, viewable.getHeight()*scale, 1);
  Square::draw(true);
  glPopMatrix();
}

void Sprite::draw (const Vector2& pos, const Vector2& size, float rotation) const {
  bind();
  glPushMatrix();
  GLW::translate(pos.x, pos.y, 0);
  GLW::rotate(RAD_TO_DEG(rotation), 0, 0, 1);
  GLW::scale(size.x, size.y, 1);
  Square::draw(true);
  glPopMatrix();
}
