#include "Button.h"

void Button::draw () const {
  if (isPressed())
    hoverSprite.draw(getPosition(), getSize());
  else
    normalSprite.draw(getPosition(), getSize());
  //drawBB(bbPos, bbSize);
}

void Button::drawPressed () const {
  hoverSprite.draw(getPosition(), getSize());
}
