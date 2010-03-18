#ifndef GLW_H_
#define GLW_H_

struct GLW {
    static void translate (float x, float y, float z) {
      glTranslatex (fX(x), fX(y), fX(z));
    }

    static void rotate (float a, float x, float y, float z) {
      glRotatex (fX(a), fX(x), fX(y), fX(z));
    }

    static void scale (float x, float y, float z) {
      glScalex (fX(x), fX(y), fX(z));
    }
};

#endif /* GLW_H_ */
