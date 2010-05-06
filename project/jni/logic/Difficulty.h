#ifndef DIFFICULTY_H_
#define DIFFICULTY_H_

#include "def.h"

/**
 * This is the class that store the settings that are difficulty-dependent
 * This is a singleton, but it should be (re)instantiated for each difficulty
 */
class Difficulty {
  public:
    static void setDifficulty (Difficulty* diff) {
      delete difficulty;
      difficulty = diff;
    }

    static Difficulty* getInstance () {
      ASSERT(difficulty);
      return difficulty;
    }

  /** Difficulty settings **/
    //Enemy tanks will observe this delay between the moment they decide to fire and the moment the
    //rocket is actually launched. During this time, they'll visually grow
    virtual double getFiringDelay () = 0;
    virtual double getBossFiringDelay () = 0;

  private:
    static Difficulty* difficulty;
};

//Easy
class DifficultyEasy : public Difficulty {
  public:
    double getFiringDelay () { return 2.0; }
    double getBossFiringDelay () { return 0.5f; }
};

#endif /* DIFFICULTY_H_ */
