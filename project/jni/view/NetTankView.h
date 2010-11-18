#ifndef _NETTANKVIEW_H
#define	_NETTANKVIEW_H

#include "logic/NetTank.h"

class NetTankView : public TankView {
  public:
    NetTankView (const NetTank* t)
      : TankView(t),
        tank(t) {}

  private:
    const NetTank* tank;
};

#endif	/* _NETTANKVIEW_H */

