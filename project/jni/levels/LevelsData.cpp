#include "LevelsData.h"


#define DECLARE_LVL \
  Level* load() { \
    return new Level(xdim,ydim,tiles,tanks,numTanks); \
  }


/**
 * LVL 0
 */
namespace lvl0 {
  const int xdim = 8;
  const int ydim = 8;

#if 0
  eTileType tiles[xdim*ydim] = {
      /*      0   1   2   3   4   5   6   7 */
      /* 0*/  E,  E,  E,  E,  E,  E,  E,  E,
      /* 1*/  E,  E,  E,  E,  E,  E,  E,  E,
      /* 2*/  E,  E,  W,  E,  E,  E,  E,  E,
      /* 3 */ E,  E,  W,  E,  E,  E,  E,  E,
      /* 4 */ E,  E,  E,  E,  E,  E,  E,  E,
      /* 5 */ E,  E,  E,  E,  E,  E,  E,  E,
      /* 6 */ E,  E,  E,  E,  E,  E,  E,  E,
      /* 7 */ E,  E,  E,  E,  E,  E,  E,  E
    };
#else
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7 */
    /* 0*/  W,  T,  T,  T,  T,  T,  T,  W,
    /* 1*/  L,  E,  E,  E,  E,  E,  E,  R,
    /* 2*/  L,  E,  W,  E,  E,  E,  E,  R,
    /* 3 */ L,  E,  E,  E,  E,  E,  E,  R,
    /* 4 */ L,  E,  R,  E,  E,  E,  E,  R,
    /* 5 */ L,  E,  R,  E,  E,  W,  E,  R,
    /* 6 */ L,  E,  E,  E,  E,  E,  E,  R,
    /* 7 */ W,  B,  B,  B,  B,  B,  B,  W
  };
#endif
  const size_t numTanks = 2;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(3, 3, TANK_STATIC, NULL),
      /*TankDescription(6, 6, TANK_STATIC, NULL)*/
  };

  DECLARE_LVL
}

/**
 * LVL 1
 */
namespace lvl1 {
  const int xdim = 8;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7 */
    /* 0*/  W,  T,  T,  T,  T,  T,  T,  W,
    /* 1*/  L,  E,  E,  E,  E,  E,  E,  R,
    /* 2*/  L,  E,  W,  E,  W,  E,  E,  R,
    /* 3 */ L,  E,  E,  E,  E,  E,  E,  R,
    /* 4 */ L,  E,  W,  E,  W,  E,  E,  R,
    /* 5 */ L,  E,  E,  E,  E,  W,  E,  R,
    /* 6 */ L,  E,  E,  E,  E,  E,  E,  R,
    /* 7 */ W,  B,  B,  B,  B,  B,  B,  W
  };

  const size_t numTanks = 3;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(3, 3, TANK_SIMPLE, NULL),
      TankDescription(6, 6, TANK_SIMPLE, NULL)
  };

  DECLARE_LVL
}

/**
 * LVL 2
 */
namespace lvl2 {
  const int xdim = 9;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8*/
    /* 0*/  W,  T,  T,  T,  T,  T,  T,  T,  W,
    /* 1*/  L,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 2*/  L,  E,  W,  E,  W,  E,  E,  E,  R,
    /* 3 */ L,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 4 */ L,  E,  W,  E,  W,  E,  E,  E,  R,
    /* 5 */ L,  E,  E,  E,  E,  E,  W,  E,  R,
    /* 6 */ L,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 7 */ W,  B,  B,  B,  B,  B,  B,  B,  W
  };

  Vector2 wp1[4] = {
       Vector2(3, 3),
       Vector2(5, 3),
       Vector2(5, 5),
       Vector2(3, 5)
  };

  Vector2 wp2[4] = {
      Vector2(7, 6),
      Vector2(7, 4),
      Vector2(5, 4),
      Vector2(5, 6)
  };

  const size_t numTanks = 3;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(3, 3, TANK_SIMPLE, new Path(4, wp1)),
      TankDescription(7, 6, TANK_SIMPLE, new Path(4, wp2))
  };

  DECLARE_LVL
}

/**
 * LVL 3
 */
namespace lvl3 {
  const int xdim = 9;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8*/
    /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  W,
    /* 1*/  L,  E,  R,  E,  E,  E,  M,  E,  R,
    /* 2*/  L,  E,  T,  E,  E,  E,  E,  E,  R,
    /* 3 */ L,  E,  E,  E,  E,  E,  M,  E,  R,
    /* 4 */ L,  E,  L,  E,  E,  E,  E,  E,  R,
    /* 5 */ L,  E,  E,  E,  E,  E,  M,  E,  R,
    /* 6 */ L,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 7 */ W,  B,  B,  B,  B,  B,  B,  B,  W
  };

  Vector2 wp1[2] = {
       Vector2(7, 1),
       Vector2(7, 6)
  };

  const size_t numTanks = 5;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(5, 2, TANK_SIMPLE, NULL),
      TankDescription(5, 4, TANK_SIMPLE, NULL),
      TankDescription(5, 6, TANK_SIMPLE, NULL),
      TankDescription(7, 1, TANK_SIMPLE, new Path(2, wp1)),
  };

  DECLARE_LVL
}

/**
 * LVL 4
 */
namespace lvl4 {
  const int xdim = 9;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8*/
    /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  W,
    /* 1*/  L,  E,  R,  E,  E,  E,  M,  E,  R,
    /* 2*/  L,  E,  T,  E,  E,  E,  E,  E,  R,
    /* 3 */ L,  E,  E,  E,  E,  E,  M,  E,  R,
    /* 4 */ L,  E,  L,  E,  E,  E,  E,  E,  R,
    /* 5 */ L,  E,  E,  E,  E,  E,  M,  E,  R,
    /* 6 */ L,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 7 */ W,  B,  B,  B,  B,  B,  B,  B,  W
  };

  Vector2 wp1[2] = {
       Vector2(7, 1),
       Vector2(7, 6)
  };

  const size_t numTanks = 3;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(5, 3, TANK_SIMPLE, NULL),
      TankDescription(7, 1, TANK_SIMPLE, new Path(2, wp1)),
  };

  DECLARE_LVL
}


/**
 * LVL 5
 */
namespace lvl5 {
  const int xdim = 9;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8*/
    /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  W,
    /* 1*/  L,  E,  R,  E,  E,  L,  E,  E,  R,
    /* 2*/  L,  E,  T,  E,  E,  E,  E,  E,  R,
    /* 3 */ L,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 4 */ L,  E,  M,  E,  E,  E,  E,  E,  R,
    /* 5 */ L,  E,  E,  E,  E,  L,  E,  E,  R,
    /* 6 */ L,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 7 */ W,  B,  B,  B,  B,  B,  B,  B,  W
  };

  Vector2 wp1[3] = {
       Vector2(3, 4),
       Vector2(6, 1),
       Vector2(6, 5)
  };

  const size_t numTanks = 2;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(4, 4, BOSS_SIMPLE, new Path(3, wp1))
  };

  DECLARE_LVL
}

/**
 * LVL 6
 */
namespace lvl6 {
  const int xdim = 9;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8*/
    /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  W,
    /* 1*/  L,  E,  R,  E,  E,  E,  E,  E,  R,
    /* 2*/  L,  E,  T,  E,  E,  E,  E,  E,  R,
    /* 3 */ L,  E,  E,  E,  E,  W,  E,  E,  R,
    /* 4 */ L,  E,  E,  E,  E,  W,  E,  E,  R,
    /* 5 */ L,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 6 */ L,  E,  E,  W,  E,  E,  E,  E,  R,
    /* 7 */ W,  B,  B,  B,  B,  B,  B,  B,  W
  };

  Vector2 wp1[2] = {
       Vector2(6, 2),
       Vector2(6, 5)
  };

  Vector2 wp2[2] = {
      Vector2(4, 6),
      Vector2(7, 6)
  };

  const size_t numTanks = 5;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(3, 1, TANK_SIMPLE, NULL),
      TankDescription(5, 1, TANK_SIMPLE, NULL),
      TankDescription(6, 2, TANK_SHIELD, new Path(2, wp1)),
      TankDescription(4, 6, TANK_SHIELD, new Path(2, wp2)),
  };

  DECLARE_LVL
}

#if 0
/**
 * LVL 4
 */

namespace lvl4 {
  const int xdim = 13;
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
namespace lvl5 {
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
namespace lvl6 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8   9  10  11 */
    /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  T,  T,  T,  W,
    /* 1*/  L,  E,  R,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    /* 2*/  W,  E,  T,  E,  E,  E,  W,  E,  E,  E,  E,  R,
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
namespace lvl7 {
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
#endif
//Array fill
const size_t numLevels = 7;
lvl_callback_t levelsLoadFns[numLevels] = {
  lvl0::load,
  lvl1::load,
  lvl2::load,
  lvl3::load,
  lvl4::load,
  lvl5::load,
  lvl6::load
};
