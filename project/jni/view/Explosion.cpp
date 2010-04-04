#include "Explosion.h"
void Explosion::draw () {
  //FIXME: Implement cosine (or cubic)interpolation
  //http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/
  //See also http://www.gamedev.net/community/forums/mod/journal/journal.asp?jn=259175&reply_id=3097072
  const float arg = (EXPLOSION_LIFE-timeLeft)/EXPLOSION_LIFE;
  //This is exponential growth
  /*static const float div = 1/expf(10);
  const float v = expf(arg*10)*div;
  const float growth = (1-v)/expf(5);

  const float size = START_SIZE + (END_SIZE-START_SIZE)*v;*/
  const float size = START_SIZE + (END_SIZE-START_SIZE)*arg;

  mainSprite.draw(position, Vector2(size, size));
}
