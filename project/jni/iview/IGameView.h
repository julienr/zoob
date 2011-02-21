#ifndef _IGAMEVIEW_H
#define	_IGAMEVIEW_H

#include "zoobdef.h"
#include "logic/Tank.h"
#include "iview/ExplosionInfo.h"

class IGameView {
  public:
    //Used by game to notify the view about tank addition/deletion
    virtual void tankAdded (Tank* t) = 0;
    virtual void tankRemoved (Tank* t) = 0;

    virtual void addExplosion (const ExplosionInfo& expl) = 0;
};


#endif	/* _IGAMEVIEW_H */

