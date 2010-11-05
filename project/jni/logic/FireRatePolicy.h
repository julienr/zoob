#ifndef FIRERATEPOLICY_H_
#define FIRERATEPOLICY_H_

#include "lib/Utils.h"

class FireRatePolicy {
  public:
    //called when the tank just fired a rocket
    virtual void fire () = 0;
    //should return wether the tank can fire
    virtual bool canFire () const = 0;

    //This can be used to indicate that the before-fire delay of this tank shouldn't be applied (when returning true)
    //because the tank governed by this policy is still in firing mode
    virtual bool isFiring () { return false; }
    virtual void cancelFiring () {}
};

class DummyFirePolicy : public FireRatePolicy {
  public:
    void fire () {}
    bool canFire () const { return false; }
};

class IntervalFirePolicy : public FireRatePolicy {
  public:
    IntervalFirePolicy (uint64_t interval)
      : fireInterval(interval),
        lastFireTime(0) {}

    void fire () {
      lastFireTime = Utils::getCurrentTimeMillis();
    }

    bool canFire () const {
      return Utils::getCurrentTimeMillis() - lastFireTime > fireInterval;
    }
  private:
    const uint64_t fireInterval; //fire interval in milliseconds
    uint64_t lastFireTime;
};

class BurstFirePolicy : public FireRatePolicy {
  public:
    /**
     * betweenBursts is the cooldown time between two bursts
     * inBurst is the cooldown time between two rockets in the same burst (should be quite small)
     * numBurst is the number of rocket in one burst
     */
    BurstFirePolicy (uint64_t betweenBursts, uint64_t inBurst, int numBurst)
      : betweenBurstsInterval(betweenBursts),
        inBurstInterval(inBurst),
        numBurst(numBurst),
        lastFireTime(0),
        burstNumLeft(0) {}

    void fire () {
      lastFireTime = Utils::getCurrentTimeMillis();
      if (burstNumLeft > 0)
        burstNumLeft--;
      else
        burstNumLeft = numBurst-1; //start a new burst
    }

    bool isFiring () {
      return burstNumLeft > 0;
    }

    void cancelFiring () {
      lastFireTime = Utils::getCurrentTimeMillis();
      burstNumLeft = 0;
    }

    bool canFire () const {
      if (burstNumLeft > 0)
        return Utils::getCurrentTimeMillis() - lastFireTime > inBurstInterval;
      else
        return Utils::getCurrentTimeMillis() - lastFireTime > betweenBurstsInterval;
    }
  private:
    const uint64_t betweenBurstsInterval;
    const uint64_t inBurstInterval;
    const int numBurst;
    uint64_t lastFireTime;
    int burstNumLeft; //number of rockets left in this burst (>0 = bursting, 0=idle)
};

#endif /* FIRERATEPOLICY_H_ */
