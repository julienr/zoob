#ifndef _PLAYERCOMMAND_H
#define _PLAYERCOMMAND_H

#include "def.h"
#include "lib/Vector2.h"

struct PlayerCommand {
  void setFire (const Vector2& dir) {
    fireDir = dir;
    fire = true;
  }

  Vector2 moveDir;
  bool fire;
  Vector2 fireDir;
  bool mine;
  bool shield; 
};

#endif
