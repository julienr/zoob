#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "def.h"
#include "view/GLW.h"

class Texture {
  public:
    Texture (const char* filename);
    ~Texture ();
    void load ();
    void unload ();

    void bind () {
      ASSERT(status == LOADED);
      glBindTexture(GL_TEXTURE_2D, glTexID);
    }

    int getWidth () {
      ASSERT(status == LOADED);
      return width;
    }

    int getHeight () {
      ASSERT(status == LOADED);
      return height;
    }
  protected:
    enum {
      LOADED,
      UNLOADED
    } status;
    GLuint glTexID;
    char* filename;
    int width, height;
};

#endif /* TEXTURE_H_ */
