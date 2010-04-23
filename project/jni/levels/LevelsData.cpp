#include "LevelsData.h"

/**
 * COL TEST
 */
eTileType coltest[8*8] = {
    W, T, T, T, T, T, T, W,
    L, E, E, W, E, E, E, R,
    L, E, E, W, E, E, E, R,
    L, E, E, E, E, E, E, R,
    L, E, E, E, E, E, E, R,
    L, E, E, W, E, E, E, R,
    L, E, E, W, E, E, E, R,
    W, B, B, B, B, B, B, W
};

TankDescription coltanks[1] = {
    TankDescription(1, 2, TANK_PLAYER, NULL)
};

Level* loadColTest() {
  return new Level(8,8,&coltest[0], coltanks, 1);
}

/**
 * LVL 1
 */
eTileType lvl1[12*8] = {
    W,  T,  T,  T,  W,  T,  T,  T,  T,  T,  T,  W,
    L,  E,  E,  E,  L,  E,  E,  L,  E,  L,  E,  R,
    W,  E,  B,  B,  W,  E,  E,  L,  E,  L,  E,  R,
    L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    L,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    L,  E,  E,  E,  E,  E,  E,  L,  E,  E,  E,  R,
    L,  E,  E,  E,  E,  E,  E,  L,  E,  E,  E,  R,
    W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
};

TankDescription lvl1tanks[6] = {
    TankDescription(1, 1, TANK_PLAYER, NULL),
    TankDescription(2, 4, TANK_SIMPLE, NULL),
    TankDescription(5, 2, TANK_SIMPLE, NULL),
    TankDescription(8, 1, TANK_SIMPLE, NULL),
    TankDescription(10, 1, TANK_SIMPLE, NULL),
    TankDescription(8, 6, TANK_SIMPLE, NULL)
};

Level* loadLevel1() {
  return new Level(12, 8, &lvl1[0], lvl1tanks, 6);
}

/**
 * LVL 2
 */

eTileType lvl2[12*8] = {
    W,  T,  T,  T,  W,  T,  T,  T,  T,  T,  T,  W,
    L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    W,  E,  B,  B,  W,  E,  E,  E,  E,  W,  E,  R,
    L,  E,  E,  E,  E,  E,  L,  E,  E,  E,  E,  R,
    L,  E,  E,  E,  E,  E,  L,  E,  E,  E,  E,  R,
    L,  E,  E,  E,  L,  E,  E,  E,  E,  W,  E,  R,
    L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
};

TankDescription lvl2tanks[5] = {
    TankDescription(1, 1, TANK_PLAYER, NULL),
    TankDescription(2, 4, TANK_SIMPLE, NULL),
    TankDescription(5, 2, TANK_SIMPLE, NULL),
    TankDescription(9, 3, TANK_SHIELD, NULL),
    TankDescription(9, 4, TANK_SHIELD, NULL)
};

Level* loadLevel2() {
  return new Level(12, 8, &lvl2[0], lvl2tanks, 5);
}

/**
 * LVL 3
 */

eTileType lvl3[12*8] = {
    W,  T,  T,  T,  W,  T,  T,  T,  T,  T,  T,  W,
    L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    W,  E,  B,  B,  W,  E,  E,  E,  E,  W,  E,  R,
    L,  E,  E,  E,  E,  E,  L,  E,  E,  E,  E,  R,
    L,  E,  E,  E,  E,  E,  L,  E,  E,  E,  E,  R,
    L,  E,  E,  E,  L,  E,  E,  E,  E,  W,  E,  R,
    L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
};

TankDescription lvl3tanks[1] = {
    TankDescription(1, 1, TANK_PLAYER, NULL)
};

Level* loadLevel3() {
  return new Level(12, 8, &lvl3[0], lvl3tanks, 1);
}


//Array fill
const size_t numLevels = 4;
lvl_callback_t levelsLoadFns[numLevels] = {
  &loadColTest,
  &loadLevel1,
  &loadLevel2,
  &loadLevel3
};
