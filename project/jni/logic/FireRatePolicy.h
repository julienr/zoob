#ifndef FIRERATEPOLICY_H_
#define FIRERATEPOLICY_H_

#include "lib/Utils.h"

class FireRatePolicy {
  public:
    //called when the tank just fired a rocket
    virtual void fire () = 0;
    //should return wether the tank can fire
    virtual bool canFire () const = 0;
};

class IntervalFirePolicy : public FireRatePolicy {
  public:
    IntervalFirePolicy (uint64_t interval)
      : fireInterval(interval) {}

    void fire () {
      lastFireTime = Utils::getCurrentTimeMillis();
    }

    bool canFire () const {
      return Utils::getCurrentTimeMillis() - lastFireTime > fireInterval;
    }
  private:
    uint64_t lastFireTime;
    const uint64_t fireInterval; //fire interval in milliseconds
};

#endif /* FIRERATEPOLICY_H_ */
