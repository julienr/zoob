#include "Font.h"
#include "view/GLW.h"
#include "def.h"

size_t* createCharLookup ();
//Characters stored in the texture, in the good order...
const char* texChars = "0123456789:=?_@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz";

//This is a table, that for a given char will give its corresponding index in texChars
//(ie charLookup[':'] will return 10)
const size_t* charLookup = createCharLookup();

//Depends on what was used in the script to create the font texture
#define TEX_SIZE 256
#define CHAR_SIZE 25
#define CHARS_PER_LINE 10.24 //256/25

size_t* createCharLookup () {
  const size_t len = strlen(texChars);
  size_t* lookup = new size_t[255];
  for (int i=0; i<255; i++) {
    for (size_t j=0; j<len; j++)
      if (texChars[j] == i)
        lookup[i] = j;
  }
  return lookup;
}

Font::Font(const char* filename) : sprite(filename) {}

void Font::print (const char* str, const Vector2& position, float size) {
  glPushMatrix();
  GLW::translate(position.x, position.y, 0);

  for (size_t i=0; i<strlen(str); i++) {
    //size_t charIdx = charLookup[(int)str[i]];
    size_t charIdx = (size_t)str[i]-32;
    const int line = charIdx/CHARS_PER_LINE;
    const int col = charIdx%(int)CHARS_PER_LINE;
    //LOGE("'%c' => col(%i), line(%i)", str[i], col, line);
    const float s = CHAR_SIZE/(float)TEX_SIZE;
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    GLW::translate((col)*s, (CHARS_PER_LINE-1-line)*s, 0);
    GLW::scale(s,s,1);
    glMatrixMode(GL_MODELVIEW);
    sprite.draw(position, Vector2(size,size));
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    GLW::translate(size, 0, 0);
  }

  glPopMatrix();
}
