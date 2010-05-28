#ifndef DIFFICULTY_H_
#define DIFFICULTY_H_

#include "def.h"

#define MAX_DIFFICULTY 2

/**
 * This is the class that store the settings that are difficulty-dependent
 * This is a singleton, but it should be (re)instantiated for each difficulty
 */
class Difficulty {
  public:
    static void setDifficulty (int diff);

    static Difficulty* getInstance () {
      ASSERT(difficulty);
      return difficulty;
    }

    int currentDifficulty () const { return difficulty->getID(); }
  /** Difficulty settings **/
    //Enemy tanks will observe this delay between the moment they decide to fire and the moment the
    //rocket is actually launched. During this time, they'll visually grow
    virtual double getFiringDelay () const = 0;
    virtual double getBossFiringDelay () const = 0;

    //Number of times a rocket should bounce before destroying a tank shield
    virtual unsigned getShieldResistance () const = 0;

  protected:
    virtual int getID () const = 0;
  private:
    static Difficulty* difficulty;
};


class DifficultyEasy : public Difficulty {
  public:
    double getFiringDelay () const { return 2.0; }
    double getBossFiringDelay () const { return 0.5f; }
    unsigned getShieldResistance () const { return 1; }
  protected:
    int getID () const { return 0; }
};

class DifficultyMedium : public Difficulty {
  public:
    double getFiringDelay () const { return 1.5; }
    double getBossFiringDelay () const { return 0.5f; }
    unsigned getShieldResistance () const { return 1; }
  protected:
    int getID () const { return 1; }
};

class DifficultyHard : public Difficulty {
  public:
    double getFiringDelay () const { return 0.7; }
    double getBossFiringDelay () const { return 0.4f; }
    unsigned getShieldResistance () const { return 2; }
  protected:
    int getID () const { return 2; }
};

#endif /* DIFFICULTY_H_ */
