#ifndef SHOOT_POLICY_H_
#define SHOOT_POLICY_H_

class ShootPolicy {
  public:
    virtual bool decideFire (double elapsedS, Vector2* outDir) = 0;
};

#endif /* SHOOT_POLICY_H_ */
