#include "NumberView.h"

NumberView* NumberView::instance = NULL;

NumberView::NumberView () {
  numbers[0] = new Sprite("assets/sprites/menuitems/digits/0.png", TEX_GROUP_UTILS);
  numbers[1] = new Sprite("assets/sprites/menuitems/digits/1.png", TEX_GROUP_UTILS);
  numbers[2] = new Sprite("assets/sprites/menuitems/digits/2.png", TEX_GROUP_UTILS);
  numbers[3] = new Sprite("assets/sprites/menuitems/digits/3.png", TEX_GROUP_UTILS);
  numbers[4] = new Sprite("assets/sprites/menuitems/digits/4.png", TEX_GROUP_UTILS);
  numbers[5] = new Sprite("assets/sprites/menuitems/digits/5.png", TEX_GROUP_UTILS);
  numbers[6] = new Sprite("assets/sprites/menuitems/digits/6.png", TEX_GROUP_UTILS);
  numbers[7] = new Sprite("assets/sprites/menuitems/digits/7.png", TEX_GROUP_UTILS);
  numbers[8] = new Sprite("assets/sprites/menuitems/digits/8.png", TEX_GROUP_UTILS);
  numbers[9] = new Sprite("assets/sprites/menuitems/digits/9.png", TEX_GROUP_UTILS);
}

NumberView::~NumberView () {
  for (int i=0; i<10; i++)
    delete numbers[i];
}

void NumberView::drawInt (const int v, const Vector2& pos, const Vector2& size) const {
  int val = v;
  int numDigits = 0;
  do {
    numDigits++;
    val /= 10;
  } while (val != 0);

  //numWidth is the visible width of the number (since most of the texture is transparent. 0.35f is
  //a hardcoded empirical constant
  float digitWidth = 0.35f*size.x;
  //pos is center, startX is rightmost digit position
  float x = pos.x+(numDigits*digitWidth)/2.0f - digitWidth/2.0f; //-digitWidth/2.0f because we want the center of the last digit
  //LOGE("numDigits : %i, digitWidth %f => x %f", numDigits, digitWidth, x);
  //draw digits, right to left
  val = v;
  do {
    const int digit = val%10;
    //LOGE("digit : %i", digit);
    numbers[digit]->draw(Vector2(x, pos.y), size);
    x -= digitWidth;
    val /= 10;
  } while (val != 0);
}
