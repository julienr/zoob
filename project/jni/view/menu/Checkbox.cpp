#include "Checkbox.h"

void Checkbox::draw() const {
  if (state == CHECKED) {
    if (isPressed())
      spriteChecked_h.draw(getPosition(), getSize());
    else
      spriteChecked.draw(getPosition(), getSize());
  } else {
    if (isPressed())
      spriteUnchecked_h.draw(getPosition(), getSize());
    else
      spriteUnchecked.draw(getPosition(), getSize());
  }
}
