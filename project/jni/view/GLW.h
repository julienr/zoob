#ifndef GLW_H_
#define GLW_H_

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
};

#endif /* GLW_H_ */
