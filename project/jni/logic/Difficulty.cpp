#include "Difficulty.h"

Difficulty* Difficulty::difficulty = NULL;

void Difficulty::setDifficulty(int diff) {
  delete difficulty;
  switch (diff) {
    case 0:
      difficulty = new DifficultyEasy();
      break;
    case 1:
      difficulty = new DifficultyMedium();
      break;
    case 2:
      difficulty = new DifficultyHard();
      break;
    default:
      ASSERT(false);
  }
}
