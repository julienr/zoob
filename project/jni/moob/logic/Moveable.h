#ifndef MOVEABLE_H_
#define MOVEABLE_H_

#include "lib/Vector2.h"
#include "containers/utlist.h"
#include "Viewable.h"

struct PositionChangeListener {
    virtual void positionChanged(const Vector2& newPosition) const = 0;
};

class Moveable : public Viewable {
  public:
    Moveable () :
      position(0,0), rotation(0), positionCallbacks(NULL) {
    }

    virtual ~Moveable () {}

    void registerPositionChangedListener(const PositionChangeListener& l) {
      LL_PREPEND(positionCallbacks, new _PosCallback(l));
      l.positionChanged(position);
    }

    void setRotation(float r) {
      rotation = r;
    }

    float getRotation() const {
      return rotation;
    }

    void setPosition (const Vector2& v) {
      position.set(v);
      notifyPosition();
    }

    void translate (const Vector2& v) {
      position.set(position.x + v.x, position.y + v.y);
      notifyPosition();
    }

    Vector2 getPosition () const {
      return position;
    }
  private:
    Vector2 position;
    float rotation; //relative to center

    void notifyPosition() {
      _PosCallback* elm;
      LL_FOREACH (positionCallbacks, elm) {
      elm->callback.positionChanged(position);
    }
  }

  struct _PosCallback {
    _PosCallback (const PositionChangeListener& c)
    : callback(c), next(NULL) {
    }
    const PositionChangeListener& callback;
    _PosCallback* next;
  };

  _PosCallback* positionCallbacks;

};

#endif /* MOVEABLE_H_ */
