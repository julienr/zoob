#ifndef _IGAMEVIEW_H
#define	_IGAMEVIEW_H

#include "def.h"
#include "logic/Tank.h"
#include "iview/ExplosionLocation.h"

class IGameView {
  public:
    //Used by game to notify the view about tank addition/deletion
    virtual void tankAdded (Tank* t) = 0;
    virtual void tankRemoved (Tank* t) = 0;

    virtual void explosion (const ExplosionLocation& expl) = 0;
};


#endif	/* _IGAMEVIEW_H */

