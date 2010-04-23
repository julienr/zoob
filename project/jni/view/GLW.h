#ifndef GLW_H_
#define GLW_H_

#include "lib/Color.h"

struct GLW {
    static void translate (float x, float y, float z) {
#ifdef PLATFORM_SDL
      glTranslatef(x,y,z);
#else
      glTranslatex (fX(x), fX(y), fX(z));
#endif
    }

    static void rotate (float a, float x, float y, float z) {
#ifdef PLATFORM_SDL
      glRotatef(a,x,y,z);
#else
      glRotatex (fX(a), fX(x), fX(y), fX(z));
#endif
    }

    static void scale (float x, float y, float z) {
#ifdef PLATFORM_SDL
      glScalef(x,y,z);
#else
      glScalex (fX(x), fX(y), fX(z));
#endif
    }

    static void ortho (float left, float right, float bottom, float top, float near, float far) {
#ifdef PLATFORM_SDL
      glOrtho(left, right, bottom, top, near, far);
#else
      glOrthof(left, right, bottom, top, near, far);
#endif
    }

    static void disableTextures () {
      glDisable(GL_TEXTURE_2D);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    static void enableTextures () {
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    static void color (eColor c) {
      switch(c) {
        case WHITE: glColor4f(1,1,1,1); break;
        case BLACK: glColor4f(0,0,0,1); break;
        case GREEN: glColor4f(0.4,1,0.24,1); break;
        case RED: glColor4f(1,0.18,0.18,1); break;
        case GREY: glColor4f(0.63,0.63,0.63,1); break;
        case TRANSPARENT_GREY: glColor4f(0.63, 0.63, 0.63, 0.8); break;
      }
    }

    static void colorWhite () {
      glColor4f(1,1,1,1);
    }

    static void checkError(const char* op) {
        for (GLint error = glGetError(); error; error = glGetError()) {
            LOGE("after %s() glError (0x%x)\n", op, error);
        }
    }
};

#endif /* GLW_H_ */
