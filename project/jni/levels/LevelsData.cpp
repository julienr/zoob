#include "LevelsData.h"

eTileType coltest[3*5] = {
    S,E,E,
    E,E,W,
    S,E,W,
    E,E,W,
    E,W,W
};

Level* loadColTest() {
  return new Level(3,5,&coltest[0]);
}


eTileType lvl1[12*8] = {
    W, T, T, T, W, T, T, T, T, T, T, W,
    L, S, E, E, L, E, E, L, S, L, S, R,
    W, B, B, E, L, S, E, L, E, L, E, R,
    L, E, E, E, L, E, E, E, E, E, E, R,
    L, E, S, E, E, E, E, E, E, E, E, R,
    L, E, E, E, E, E, E, L, E, E, E, R,
    L, E, E, E, E, E, E, L, S, E, E, R,
    W, B, B, B, B, B, B, B, B, B, B, W,
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
