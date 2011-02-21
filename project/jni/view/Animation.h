#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "lib/Utils.h"
#include "zoobdef.h"
#include "view/textures/Texture.h"

class Animation {
  public:
    struct Interpolator {
      //t is between 0 and 1, where 0 is the start of the animation and 1 the end
      //should return a value between 0 and 1
      virtual float interpolate (float t) = 0;
    };

    struct LinearInterpolator : public Interpolator {
      float interpolate (float t) { return t; }
    };

    //Starts slowly, ends fast
    struct AccelerateInterpolator : public Interpolator {
      AccelerateInterpolator (float factor=1.0f) 
        : factor(factor),doubleFactor(2*factor) {}
      float interpolate (float t) {
        if (factor == 1.0f)
          return t*t;
        else
          return powf(t, doubleFactor);
      }
      private:
        const float factor;
        const float doubleFactor;
    };

    //Starts fast, ends slowly
    struct DecelerateInterpolator : public Interpolator {
      DecelerateInterpolator (float factor=1.0f) 
        : factor(factor),doubleFactor(2*factor) {}
      float interpolate (float t) {
        if (factor == 1.0f)
          return 1.0f - (1.0f-t)*(1.0f-t);
        else
          return 1.0f - powf(1.0f-t, doubleFactor);
      }
      private:
        const float factor;
        const float doubleFactor;
    };

  public:
    //A class that handle animation based on a texture atlas.
    //The atlas is supposed to have numFrames, distributed in sqrt(numFrames) rows and cols 
    //The animation takes ownership of the interpolator
    Animation (const char* filename, int groupID, int numFrames, Interpolator* inter, float animTime, bool loop=true);

    ~Animation () {
      delete interpolator;
    }

    void start();
    void draw (const Vector2& position, const Vector2& size, float elapsedS);

    bool isActive () const { return active; }
  private:
    bool active;
    Texture* texture;
    const int numFrames;
    const float animTime;
    const bool loop;
    Interpolator* interpolator;

    int atlasSize; //sqrt(numFrames)
    const float texScale;

    float timeSpent; //how much time already spent on the anim (reseted at each loop)
};

#endif
