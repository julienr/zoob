#ifndef GLW_H_
#define GLW_H_

#include "lib/Vector2.h"
#include "Color.h"
#include "def.h"

struct GLW {
    static void translate (float x, float y, float z) {
#ifdef PLATFORM_SDL
      glTranslatef(x,y,z);
#else
      glTranslatex (fX(x), fX(y), fX(z));
#endif
    }

    static void translate (const Vector2& v) {
      translate(v.x,v.y,0);
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

    static void color (eColor c, float a=1.0f) {
      switch(c) {
        case WHITE: glColor4f(1,1,1,a); break;
        case BLACK: glColor4f(0,0,0,a); break;
        case GREEN: glColor4f(0.4,1,0.24,a); break;
        case RED: glColor4f(1,0.18,0.18,a); break;
        case GREY: glColor4f(0.63,0.63,0.63,a); break;
        case DARK_GREY: glColor4f(0.4f,0.4f,0.4f,a); break;
        case ORANGE: glColor4f(1, 0.51, 0.18, a); break;
        case VIOLET: glColor4f(0.97, 0.18, 1, a); break;
        case YELLOW: glColor4f(1, 0.97f, 0.18f, a); break;
        case BLUE: glColor4f(0.23f, 0.30f, 1, a); break;
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

  private:
    static bool isExtensionSupported(const char *extension);
};

#endif /* GLW_H_ */
