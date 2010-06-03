#ifndef FADEANIM_H_
#define FADEANIM_H_

//An animation with fading between the two sprites being animated
class FadeAnim {
  public:
    /** A fading animation between sprite1 and sprite2 (and back).
     * fadingTime is the time taken to fade from sprite1 to sprite2
     * pauseTime is the time for which the animation is paused (for sprite1 and sprite 2)
     */
    FadeAnim (char* sprite1, char* sprite2, double fadingTime, double pause1, double pause2)
      : sprite1(sprite1, TEX_GROUP_GAME),
        sprite2(sprite2, TEX_GROUP_GAME),
        fadingTime(fadingTime),
        pause1(pause1),
        pause2(pause2),
        state(SPRITE1_PAUSE)
        {}

    void think (double elapsedS) {
      stateTimeLeft -= elapsedS;
      if (stateTimeLeft <=0 ) {
        switch (state) {
          case SPRITE1_PAUSE:
            state = SPRITE1_TO_SPRITE2;
            stateTimeLeft = fadingTime;
            break;
          case SPRITE2_PAUSE:
            state = SPRITE2_TO_SPRITE1;
            stateTimeLeft = fadingTime;
            break;
          case SPRITE1_TO_SPRITE2:
            state = SPRITE2_PAUSE;
            stateTimeLeft = pause2;
            break;
          case SPRITE2_TO_SPRITE1:
            state = SPRITE1_PAUSE;
            stateTimeLeft = pause1;
            break;
        }
      }
    }

    void draw (const Viewable& viewable, float rotation=0, float scale=1) {
      switch (state) {
        case SPRITE1_PAUSE:
          sprite1.draw(viewable, rotation, scale);
          break;
        case SPRITE2_PAUSE:
          sprite2.draw(viewable, rotation, scale);
          break;
        case SPRITE1_TO_SPRITE2:
          glColor4f(1,1,1,stateTimeLeft/fadingTime);
          sprite1.draw(viewable, rotation, scale);
          glColor4f(1,1,1,(fadingTime-stateTimeLeft)/fadingTime);
          sprite2.draw(viewable, rotation, scale);
          glColor4f(1,1,1,1);
          break;
        case SPRITE2_TO_SPRITE1:
          glColor4f(1,1,1,stateTimeLeft/fadingTime);
          sprite2.draw(viewable, rotation, scale);
          glColor4f(1,1,1,(fadingTime-stateTimeLeft)/fadingTime);
          sprite1.draw(viewable, rotation, scale);
          glColor4f(1,1,1,1);
          break;
      }
    }

  private:
    double stateTimeLeft;
    const Sprite sprite1;
    const Sprite sprite2;
    const double fadingTime;
    const double pause1;
    const double pause2;

    enum State {
        SPRITE1_PAUSE=0,
        SPRITE2_PAUSE,
        SPRITE1_TO_SPRITE2,
        SPRITE2_TO_SPRITE1
    };

    State state;
};

#endif /* FADEANIM_H_ */
