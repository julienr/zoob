#include "Sprite.h"
#include "view/GLW.h"
#include "Square.h"

void Sprite::draw (const Viewable& viewable, float rotation) const {
  //FIXME: do something if textureID = NO_TEXTURE
  glBindTexture(GL_TEXTURE_2D, textureID);
  glPushMatrix();
  GLW::scale(viewable.getWidth(), viewable.getHeight(), 1);
  GLW::translate(viewable.getPosition().x, viewable.getPosition().y, 0);
  GLW::rotate(RAD_TO_DEG(rotation), 0, 0, 1);
  Square::draw(true);
  glPopMatrix();
}

void Sprite::draw (const Vector2& pos, const Vector2& size, float rotation) const {
  //FIXME: do something if textureID = NO_TEXTURE
  glBindTexture(GL_TEXTURE_2D, textureID);
  glPushMatrix();
  GLW::scale(size.x, size.y, 1);
  GLW::translate(pos.x, pos.y, 0);
  GLW::rotate(RAD_TO_DEG(rotation), 0, 0, 1);
  Square::draw(true);
  glPopMatrix();
}
