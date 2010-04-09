#ifndef FONT_H_
#define FONT_H_

#include "view/Sprite.h"
#include "lib/Vector2.h"

class Font {
  public:
    Font (const char* filename);

    //Draw str at position with a single character being of size "size"
    void print (const char* str, const Vector2& position, float size);
  private:
    Sprite sprite;
};

#endif /* FONT_H_ */
