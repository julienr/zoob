#ifndef SPRITE_H_
#define SPRITE_H_

#include "TextureManager.h"
#include "logic/Viewable.h"
#include "lib/Utils.h"
#include "containers/list.h"

class Sprite {
  /** On android, when the application goes to sleep mode, the OpenGL context is lost and all
   * resources (such as textures) must be reloaded. To handle this, we need a way to tell all sprites
   * that they should reload their textureID. This is done by maintaning a list of all the sprites objects
   */
  private:
    static list<Sprite*> globalSpritesList;
  public:
    static void reloadAllSprites ();

  public:
    Sprite (const char* filename) 
     : file(filename) {
      reloadTexture();
      globalSpritesList.append(this);
    }

    ~Sprite () {
      globalSpritesList.remove(this);
    }

    void reloadTexture () {
      textureID = TextureManager::getInstance()->get(file);
    }

    void draw (const Viewable& viewable, float rotation=0, float scale=1) const;
    void draw (const Vector2& pos, const Vector2& size, float rotation=0) const;

    Vector2 getCenter (const Viewable& viewable) const {
      return Vector2(viewable.getPosition().x+viewable.getWidth()/2.0f,
                      viewable.getPosition().y+viewable.getHeight()/2.0f);
    }

    //Since this is for touch use, double the area
    bool touchInside (const Viewable& viewable, const Vector2& p) const {
      const Vector2 ext = viewable.getSize()*2;
      return Utils::insideTL(viewable.getPosition()-(ext/2), ext.x, ext.y, p);
    }
  private:
    const char* file;
    GLuint textureID;
};

#endif /* SPRITE_H_ */
