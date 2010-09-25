#include "Animation.h"
#include "view/textures/TextureManager.h"
#include "view/primitives/Square.h"

Animation::Animation (const char* filename,
                      int groupID,
                      int numFrames, 
                      Interpolator* inter, 
                      float animTime,
                      bool loop)
  : active(false),
    texture(TextureManager::getInstance()->getGroup(groupID)->get(filename)), 
    numFrames(numFrames), 
    animTime(animTime),
    loop(loop),
    interpolator(inter),
    atlasSize(sqrtf(numFrames)),
    texScale(1.0f/atlasSize),
    timeSpent(0) {
}

void Animation::start() {
  timeSpent=0;
  active=true;
}

void Animation::draw (const Vector2& position, const Vector2& size, float elapsedS) {
  if (!active)
    return;

  timeSpent += elapsedS;
  if (timeSpent > animTime) {
    if (loop)
      timeSpent=0;
    else
      active = false;
  }

  const float t = interpolator->interpolate(timeSpent/animTime);
  const int currentFrame = t*numFrames; 
  //LOGE("currentFrame : %i", currentFrame);

  texture->bind();

  //Looks like some android devices don't really support the texture matrix.
  //Therefore, we have to change the texcoords 
  
  const float u = currentFrame%atlasSize;
  const float v = (atlasSize-1)-currentFrame/atlasSize;
  MGL_DATATYPE texCoords[8] = {
    fX(texScale*u), fX(texScale*(v+1)),
    fX(texScale*(u+1)), fX(texScale*(v+1)),
    fX(texScale*u), fX(texScale*v),
    fX(texScale*(u+1)),fX(texScale*v)
  };

/*  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glScalef(texScale, texScale, 1);
  const float u = currentFrame%atlasSize;
  const float v = (atlasSize-1)-currentFrame/atlasSize;
  glTranslatef(u, v, 0);

  glMatrixMode(GL_MODELVIEW);*/
  glPushMatrix();
  GLW::translate(position);
  GLW::scale(size);

  glTexCoordPointer(2, MGL_TYPE, 0, texCoords);
  Square::draw(false);
  glPopMatrix();

/*  glMatrixMode(GL_TEXTURE);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);*/
}
