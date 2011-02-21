#ifndef _PLAYERCOMMAND_H
#define _PLAYERCOMMAND_H

#include "zoobdef.h"
#include "lib/Vector2.h"

struct PlayerCommand {
  PlayerCommand () :moveDir(0,0),fire(false),fireDir(0,0),mine(false),shield(false) {}
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
