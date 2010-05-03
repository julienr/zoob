#include "LevelsData.h"


#define DECLARE_LVL \
  Level* load() { \
    return new Level(xdim,ydim,tiles,tanks,numTanks); \
  }


/**
 * COL TEST
 */
namespace coltest {
  const int xdim = 8;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
      W, T, T, W, T, W, T, W,
      L, E, E, E, E, E, E, R,
      L, E, W, E, E, E, E, W,
      L, E, E, E, E, E, E, R,
      L, E, E, E, E, E, E, R,
      L, E, E, W, W, W, E, R,
      L, E, E, W, E, E, E, R,
      W, B, B, W, B, B, B, W
  };

  const size_t numTanks = 2;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(/*4, 6*/3,3, TANK_STATIC, NULL)
  };

  DECLARE_LVL
}

/**
 * LVL 1
 */
namespace lvl1 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[12*8] = {
      W,  T,  T,  T,  W,  T,  T,  T,  T,  T,  T,  W,
      L,  E,  E,  E,  L,  E,  E,  L,  E,  L,  E,  R,
      W,  E,  B,  B,  W,  E,  E,  L,  E,  L,  E,  R,
      L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
      L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
      L,  E,  E,  E,  E,  E,  E,  L,  E,  E,  E,  R,
      L,  E,  E,  E,  E,  E,  E,  L,  E,  E,  E,  R,
      W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
  };

  const size_t numTanks = 6;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(2, 4, TANK_SIMPLE, NULL),
      TankDescription(5, 2, TANK_SIMPLE, NULL),
      TankDescription(8, 1, TANK_SIMPLE, NULL),
      TankDescription(10, 1, TANK_SIMPLE, NULL),
      TankDescription(8, 6, TANK_SIMPLE, NULL)
  };

  DECLARE_LVL
}

/**
 * LVL 2
 */

namespace lvl2 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
      W,  T,  T,  T,  W,  T,  T,  T,  T,  T,  T,  W,
      L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
      W,  E,  B,  B,  W,  E,  E,  E,  E,  W,  E,  R,
      L,  E,  E,  E,  E,  E,  L,  E,  E,  E,  E,  R,
      L,  E,  E,  E,  E,  E,  L,  E,  E,  E,  E,  R,
      L,  E,  E,  E,  L,  E,  E,  E,  E,  W,  E,  R,
      L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
      W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
  };

  const size_t numTanks = 5;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(2, 4, TANK_SIMPLE, NULL),
      TankDescription(5, 2, TANK_SIMPLE, NULL),
      TankDescription(9, 3, TANK_SHIELD, NULL),
      TankDescription(9, 4, TANK_SHIELD, NULL)
  };

  DECLARE_LVL
}

/**
 * LVL 3
 */
namespace lvl3 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8   9  10  11 */
    /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  T,  T,  T,  W,
    /* 1*/  W,  E,  W,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 2*/  W,  E,  W,  E,  E,  E,  W,  E,  E,  E,  E,  R,
    /* 3*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 4*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 5*/  L,  E,  E,  E,  L,  E,  W,  E,  E,  E,  E,  R,
    /* 6*/  L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    /* 7*/  W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
  };

  Vector2 wp1[4] = {
      Vector2(5, 1),
      Vector2(5, 3),
      Vector2(7, 3),
      Vector2(7, 1)
  };

  Vector2 wp2[4] = {
      Vector2(7, 4),
      Vector2(7, 6),
      Vector2(5, 6),
      Vector2(5, 4)
  };

  const size_t numTanks = 3;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(5, 1, TANK_SIMPLE, new Path(4, wp1)),
      TankDescription(7, 4, TANK_SIMPLE, new Path(4, wp2))
  };

  DECLARE_LVL
}

/**
 * LVL 4
 */
namespace lvl4 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8   9  10  11 */
    /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  T,  T,  T,  W,
    /* 1*/  W,  E,  W,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 2*/  W,  E,  W,  E,  E,  E,  W,  E,  E,  E,  E,  R,
    /* 3*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 4*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 5*/  L,  E,  E,  E,  L,  E,  W,  E,  E,  E,  E,  R,
    /* 6*/  L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    /* 7*/  W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
  };

  Vector2 wp1[4] = {
      Vector2(5, 1),
      Vector2(5, 3),
      Vector2(7, 3),
      Vector2(7, 1)
  };

  Vector2 wp2[4] = {
      Vector2(7, 4),
      Vector2(7, 6),
      Vector2(5, 6),
      Vector2(5, 4)
  };

  const size_t numTanks = 3;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(5, 1, TANK_SHIELD, new Path(4, wp1)),
      TankDescription(7, 4, TANK_BURST, new Path(4, wp2))
  };

  DECLARE_LVL
}

/**
 * LVL 5
 */
namespace lvl5 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8   9  10  11 */
    /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  T,  T,  T,  W,
    /* 1*/  W,  E,  W,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 2*/  W,  E,  W,  E,  E,  B,  B,  B,  B,  E,  E,  R,
    /* 3*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 4*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 5*/  L,  E,  E,  E,  E,  T,  T,  T,  T,  E,  E,  R,
    /* 6*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 7*/  W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
  };

  Vector2 wp1[2] = {
      Vector2(5, 1),
      Vector2(8, 1)
  };

  Vector2 wp2[2] = {
      Vector2(5, 6),
      Vector2(8, 6)
  };

  const size_t numTanks = 5;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(5, 1, TANK_SIMPLE, new Path(2, wp1)),
      TankDescription(8, 6, TANK_SIMPLE, new Path(2, wp2)),
      TankDescription(8, 3, TANK_SHIELD, NULL),
      TankDescription(8, 4, TANK_SHIELD, NULL)
  };

  DECLARE_LVL
}

//Array fill
const size_t numLevels = 6;
lvl_callback_t levelsLoadFns[numLevels] = {
  coltest::load,
  lvl1::load,
  lvl2::load,
  lvl3::load,
  lvl4::load,
  lvl5::load
};
