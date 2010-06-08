#include "LevelsData.h"

#define DECLARE_LVL \
  Level* load() { \
    return new Level(xdim,ydim,tiles,tanks,numTanks,shadows,boss); \
  }


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
  bool shadows = false;
  bool boss = false;
  const size_t numTanks = 2;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(3, 3, TANK_STATIC, NULL),
      /*TankDescription(6, 6, TANK_STATIC, NULL)*/
  };

  DECLARE_LVL
}

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

  bool shadows = false;
  bool boss = false;
  const size_t numTanks = 3;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(3, 3, TANK_SIMPLE, NULL),
      TankDescription(6, 6, TANK_SIMPLE, NULL)
  };

  DECLARE_LVL
}

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

  Vector2 wp2[2] = {
      Vector2(7, 5),
      Vector2(7, 1)
  };

  Vector2 wp3[2] = {
        Vector2(6, 6),
        Vector2(1, 6)
  };

  bool shadows = false;
  bool boss = false;
  const size_t numTanks = 4;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(3, 3, TANK_SIMPLE, new Path(4, wp1)),
      TankDescription(7, 5, TANK_SIMPLE, new Path(2, wp2)),
      TankDescription(6, 6, TANK_SIMPLE, new Path(2, wp3))
  };

  DECLARE_LVL
}

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

  bool shadows = false;
  bool boss = false;
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

  bool shadows = false;
  bool boss = false;
  const size_t numTanks = 3;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(5, 3, TANK_SIMPLE, NULL),
      TankDescription(7, 1, TANK_SIMPLE, new Path(2, wp1)),
  };

  DECLARE_LVL
}

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

  bool shadows = true;
  bool boss = true;
  const size_t numTanks = 2;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(4, 4, BOSS_SIMPLE, NULL)
  };

  DECLARE_LVL
}

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

  bool shadows = false;
  bool boss = false;
  const size_t numTanks = 5;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(3, 1, TANK_SIMPLE, NULL),
      TankDescription(5, 1, TANK_SIMPLE, NULL),
      TankDescription(6, 2, TANK_BOUNCE, new Path(2, wp1)),
      TankDescription(4, 6, TANK_BOUNCE, new Path(2, wp2)),
  };

  DECLARE_LVL
}

namespace lvl7 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
    /*      0   1   2   3   4   5   6   7   8   9  10  11 */
    /* 0 */ W,  T,  T,  T,  W,  T,  T,  T,  T,  T,  T,  W,
    /* 1 */ L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    /* 2 */ W,  E,  B,  B,  W,  E,  E,  E,  E,  W,  E,  R,
    /* 3 */ L,  E,  E,  E,  E,  E,  L,  E,  E,  E,  E,  R,
    /* 4 */ L,  E,  E,  E,  E,  E,  L,  E,  E,  E,  E,  R,
    /* 5 */ L,  E,  E,  E,  L,  E,  E,  E,  E,  W,  E,  R,
    /* 6 */ L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    /* 7 */ W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
  };

  bool shadows=false;
  bool boss = false;
  const size_t numTanks = 5;
  TankDescription tanks[numTanks] = {
      TankDescription(1, 1, TANK_PLAYER, NULL),
      TankDescription(2, 4, TANK_SIMPLE, NULL),
      TankDescription(5, 2, TANK_SIMPLE, NULL),
      TankDescription(9, 3, TANK_BOUNCE, NULL),
      TankDescription(9, 4, TANK_BOUNCE, NULL)
  };

  DECLARE_LVL
}


namespace lvl8 {
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

  bool shadows=false;
  bool boss = false;
  const size_t numTanks = 5;
  TankDescription tanks[numTanks] = {
     TankDescription(1, 1, TANK_PLAYER, NULL),
     TankDescription(5, 1, TANK_SIMPLE, new Path(2, wp1)),
     TankDescription(8, 6, TANK_SIMPLE, new Path(2, wp2)),
     TankDescription(8, 3, TANK_BOUNCE, NULL),
     TankDescription(8, 4, TANK_BOUNCE, NULL)
  };

  DECLARE_LVL
}

namespace lvl9 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
     /*      0   1   2   3   4   5   6   7   8   9  10  11 */
     /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  T,  T,  T,  W,
     /* 1*/  W,  E,  R,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 2*/  W,  E,  R,  E,  E,  M,  E,  E,  M,  E,  E,  R,
     /* 3*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 4*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 5*/  L,  E,  E,  E,  E,  M,  E,  E,  M,  E,  E,  R,
     /* 6*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 7*/  W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
  };

  Vector2 wp1[4] = {
     Vector2(6,2),
     Vector2(5,1),
     Vector2(4,2),
     Vector2(5,3)
  };

  Vector2 wp2[4] = {
     Vector2(5,4),
     Vector2(6,5),
     Vector2(5,6),
     Vector2(4,5)
  };

  Vector2 wp3[2] = {
     Vector2(10, 2),
     Vector2(7, 3)
  };

  Vector2 wp4[2] = {
     Vector2(3, 1),
     Vector2(3, 3)
  };

  bool shadows=false;
  bool boss = false;
  const size_t numTanks = 5;
  TankDescription tanks[numTanks] = {
     TankDescription(1, 1, TANK_PLAYER, NULL),
     TankDescription(6, 2, TANK_SIMPLE, new Path(4, wp1)),
     TankDescription(5, 4, TANK_SIMPLE, new Path(4, wp2)),
     TankDescription(10, 2, TANK_BOUNCE, new Path(2, wp3)),
     TankDescription(3, 1, TANK_BOUNCE, new Path(2, wp4))
  };

  DECLARE_LVL
}

namespace lvl10 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
     /*      0   1   2   3   4   5   6   7   8   9  10  11 */
     /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  T,  T,  T,  W,
     /* 1*/  W,  E,  R,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 2*/  W,  E,  R,  E,  E,  M,  E,  E,  M,  E,  E,  R,
     /* 3*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 4*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 5*/  L,  E,  E,  E,  E,  M,  E,  E,  M,  E,  E,  R,
     /* 6*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 7*/  W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
  };

  bool shadows=true;
  bool boss = true;
  const size_t numTanks = 2;
  TankDescription tanks[numTanks] = {
     TankDescription(1, 1, TANK_PLAYER, NULL),
     TankDescription(7, 4, BOSS_BOUNCE, NULL)
  };

  DECLARE_LVL
}

namespace lvl11 {
  const int xdim = 12;
  const int ydim = 8;
  eTileType tiles[xdim*ydim] = {
     /*      0   1   2   3   4   5   6   7   8   9  10  11 */
     /* 0*/  W,  T,  W,  T,  T,  T,  T,  T,  T,  T,  T,  W,
     /* 1*/  W,  E,  R,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 2*/  W,  E,  R,  E,  E,  M,  E,  E,  M,  E,  E,  R,
     /* 3*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 4*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 5*/  L,  E,  E,  E,  E,  M,  E,  E,  M,  E,  E,  R,
     /* 6*/  L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
     /* 7*/  W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
  };

  Vector2 wp1[2] = {
        Vector2(6, 2),
        Vector2(6, 5)
   };

   Vector2 wp2[2] = {
       Vector2(4, 6),
       Vector2(7, 6)
   };

   bool shadows = false;
   bool boss = false;
   const size_t numTanks = 5;
   TankDescription tanks[numTanks] = {
       TankDescription(1, 1, TANK_PLAYER, NULL),
       TankDescription(3, 1, TANK_BOUNCE, NULL),
       TankDescription(5, 1, TANK_BOUNCE, NULL),
       TankDescription(6, 2, TANK_BOUNCE, new Path(2, wp1)),
       TankDescription(4, 6, TANK_SHIELD, new Path(2, wp2)),
   };

  DECLARE_LVL
}

#include "levelgen.h"
#include "containers/pair.h"

namespace lvl12 {
  const int width = 12;
  const int height = 8;

  Level* load() {
    eTileType* tiles = generateLevel(width, height);
    vector<pair<int,int> > starting(2);
    size_t c = 0;
    for (int x=0; x<width; x++) {
      for (int y=0; y<height; y++) {
        if (tiles[y*width+x] == E) {
          if (c < 2) {
            starting.append(pair<int,int>(x,y));
            c++;
          } else
            break;
        }
      }
      if (c == 2)
        break;
    }
    ASSERT(c==2);
    LOGE("starting 1 (%i,%i)", starting[0].first, starting[0].second);
    LOGE("starting 2 (%i,%i)", starting[1].first, starting[1].second);
    const size_t numTanks = 2;

    TankDescription* tanks = new TankDescription[numTanks];
    tanks[0] = TankDescription(starting[0].first, starting[0].second, TANK_PLAYER, NULL);
    tanks[1] = TankDescription(starting[1].first, starting[1].second, TANK_STATIC, NULL);
    return new Level(width,height,generateLevel (width, height),tanks,numTanks,false,false);
  }
}

//Array fill
const size_t numLevels = 13;
lvl_callback_t levelsLoadFns[numLevels] = {
  lvl0::load,
  lvl1::load,
  lvl2::load,
  lvl3::load,
  lvl4::load,
  lvl5::load,
  lvl6::load,
  lvl7::load,
  lvl8::load,
  lvl9::load,
  lvl10::load,
  lvl11::load,
  lvl12::load
};
