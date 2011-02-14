#ifndef MOVEABLE_H_
#define MOVEABLE_H_

#include "lib/Vector2.h"
#include "containers/utlist.h"
#include "Viewable.h"

class Moveable : public Viewable {
  public:
    Moveable () :
      Viewable(), position(0,0), rotation(0) {
    }

    Moveable (const Vector2& pos) :
      Viewable(), position(pos), rotation(0) {
    }

    virtual ~Moveable () {}

    void setRotation(float r) {
      rotation = r;
    }

    //dir MUST be normalized
    virtual void setRotationFromDir (const Vector2& dir) {
      const Vector2 yAxis = Vector2(0,1);
      const float angle = acos(dir*yAxis) * yAxis.relativeOrientation(dir);
      setRotation(angle);
    }

    float getRotation() const {
      return rotation;
    }

    void setPosition (const Vector2& v) {
      position.set(v);
    }

    void translate (const Vector2& v) {
      position.set(position.x + v.x, position.y + v.y);
    }

    const Vector2& getPosition () const {
      return position;
    }

    //See comment below on lastSafePosition. Should only be used for collision handling
    const Vector2& getLastSafePosition () const {
      return lastSafePosition;
    }

    //save the current position as the last safe position
    void saveSafePosition () {
      lastSafePosition = position;
    }
  private:
    Vector2 position;
    float rotation; //relative to center


    //This is used in collision handling. In case this moveable is found to be stuck, its last safe position will be restored
    //this last safe position should NEVER be used for something outside of collision handling
    Vector2 lastSafePosition;
};

#endif /* MOVEABLE_H_ */
