#ifndef RANDOMAI_H_
#define RANDOMAI_H_

#define FIRE_INTERVAL 1.0f
#define FIRE_ROTATION_SPEED 0.5f

class RandomAI : public TankAI {
  public:
    RandomAI () : TankAI(), timeSinceFire(0), angle(0) {}
    bool decideFire (double elapsedS, Vector2* outDir) {
      timeSinceFire += elapsedS;
      angle += elapsedS*FIRE_ROTATION_SPEED;
      if (timeSinceFire > FIRE_INTERVAL) {
        timeSinceFire = 0;
        outDir->set(cosf(angle), sinf(angle));
        return true;
      }
      return false;
    }

    Vector2 decideDir (double elapsedS) {
      return Vector2(0,0);
    }

  private:
    double timeSinceFire;
    float angle;
};

#endif /* RANDOMAI_H_ */
