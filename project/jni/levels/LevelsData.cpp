#include "LevelsData.h"

eTileType coltest[8*8] = {
    W, T, T, T, T, T, T, W,
    L, E, E, E, E, E, E, R,
    L, S, E, E, E, E, E, R,
    L, E, E, E, E, E, E, R,
    L, E, E, E, E, _1, E, R,
    L, E, E, W, E, E, E, R,
    L, E, E, W, E, E, E, R,
    W, B, B, B, B, B, B, W
};

Level* loadColTest() {
  return new Level(8,8,&coltest[0]);
}


eTileType lvl1[12*8] = {
    W,  T,  T,  T,  W,  T,  T,  T,  T,  T,  T,  W,
    L,  S,  E,  E,  L,  E,  E,  L, _1,  L, _1,  R,
    W,  B,  B,  E,  L, _1,  E,  L,  E,  L,  E,  R,
    L,  E,  E,  E,  L,  E,  E,  E,  E,  E,  E,  R,
    L,  E, _1,  E,  E,  E,  E,  E,  E,  E,  E,  R,
    L,  E,  E,  E,  E,  E,  E,  L,  E,  E,  E,  R,
    L,  E,  E,  E,  E,  E,  E,  L, _1,  E,  E,  R,
    W,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  W,
};

Level* loadLevel1() {
  return new Level(12, 8, &lvl1[0]);
}

eTileType lvl2[8*8] = {
    W, T, T, T, T, T, T, W,
    L, E, E, E, E, E, E, R,
    L, S, E, S, E, E, E, R,
    L, E, E, E, R, E, E, R,
    L, E, T, T, W, E, E, R,
    L, E, E, E, E, E, E, R,
    L, E, E, E, E, E, E, R,
    W, B, B, B, B, B, B, W
};

Level* loadLevel2() {
  return new Level(8, 8, &lvl2[0]);
}

//Array fill
const size_t numLevels = 3;
lvl_callback_t levelsLoadFns[numLevels] = {
  &loadColTest,
  &loadLevel1,
  &loadLevel2
};
