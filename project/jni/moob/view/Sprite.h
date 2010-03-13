#ifndef SPRITE_H_
#define SPRITE_H_

#include "TextureManager.h"
#include "logic/Viewable.h"

class Sprite {
  public:
    Sprite (const char* filename) {
      textureID = TextureManager::getInstance()->get(filename);
    }

    void draw (const Viewable& viewable, float rotation=0) const;
    void draw (const Vector2& pos, const Vector2& size, float rotation=0) const;

  private:
    GLuint textureID;
};

#endif /* SPRITE_H_ */
