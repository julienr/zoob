#include "Sprite.h"
#include "view/GLW.h"
#include "Square.h"

void Sprite::draw (const Viewable& viewable, float rotation) const {
  const Vector2& p = viewable.getPosition();
  //FIXME: do something if textureID = NO_TEXTURE
  glBindTexture(GL_TEXTURE_2D, textureID);
  glPushMatrix();
  GLW::translate(p.x, p.y, 0);
  GLW::rotate(RAD_TO_DEG(rotation), 0, 0, 1);
  GLW::scale(viewable.getWidth(), viewable.getHeight(), 0);
  Square::draw(true);
  glPopMatrix();
}

void Sprite::draw (const Vector2& pos, const Vector2& size, float rotation) const {
  //FIXME: do something if textureID = NO_TEXTURE
  glBindTexture(GL_TEXTURE_2D, textureID);
  glPushMatrix();
  GLW::translate(pos.x, pos.y, 0);
  GLW::rotate(RAD_TO_DEG(rotation), 0, 0, 1);
  GLW::scale(size.x, size.y, 0);
  Square::draw(true);
  glPopMatrix();
}
