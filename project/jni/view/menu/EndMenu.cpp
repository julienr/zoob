#include "EndMenu.h"
#include "view/GameManager.h"

EndMenu::EndMenu (GameManager* gm)
  : Menu (gm),
    complete("assets/sprites/menuitems/all_complete.png"),
    starSprite("assets/sprites/menuitems/star.png"),
    thx("assets/sprites/menuitems/thx_playing.png") {
  stars[0] = new _Star(Vector2(13,2), Vector2(2,2), 0, 0);
  stars[1] = new _Star(Vector2(8, 5), Vector2(2.5, 2.5), 0, 0.2);
  stars[2] = new _Star(Vector2(3, 6), Vector2(2,2), 0, 0);
  lastUpdate = Utils::getCurrentTimeMillis();
}

void EndMenu::draw () {
  const uint64_t now = Utils::getCurrentTimeMillis();
  float elapsedS = ((int)(lastUpdate-now))/1000.0f;
  lastUpdate = now;

  complete.draw(Vector2(7.5,2), Vector2(8,4));
  thx.draw(Vector2(7.5, 8), Vector2(8,2));

  for (int i=0; i<NUM_STARS; i++) {
    stars[i]->rot += elapsedS*stars[i]->rotSpeed;
    starSprite.draw(stars[i]->pos, stars[i]->size, stars[i]->rot);
  }
}

void EndMenu::actionPerformed () {
  gameManager->goToMenu();
}
