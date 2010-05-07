#include "DefensivePolicy.h"
#include "ai/algorithms/AStar.h"
#include "logic/Game.h"

bool DefensivePolicy::decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  const Vector2& myPos = tank->getPosition();
  const Vector2& playerPos = game->getPlayerTank().getPosition();
  const CollisionManager& colManager = game->getColManager();

  Vector2 pos[5] = {myPos, myPos+Vector2(1.0f,0), myPos-Vector2(1.0f,0),
                           myPos+Vector2(0,1.0f), myPos-Vector2(0,1.0f)};
  int maxI = 0;
  float dist = (pos[maxI] - playerPos).length();

  //LOGE("--pos (%f,%f), dist=%f", myPos.x, myPos.y, dist);
  for (int i=1; i<5; i++) {
    CollisionResult r;
    float d;
    if (colManager.trace(tank, pos[i]-myPos, &r))
      d = (r.colPoint - playerPos).length();
    else
      d = (pos[i]-playerPos).length();
    //LOGE("i:%i, colPoint(%f,%f), d=%f", i, r.colPoint.x, r.colPoint.y, d);
    if (d > dist) {
      dist = d;
      maxI = i;
    }
  }

  //LOGE("maxI : %i", maxI);
  if (maxI == 0)
    return false;
  outDir->set((pos[maxI] - myPos).getNormalized());
  return true;
}
