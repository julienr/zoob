#include "GameView.h"

void GameView::draw () {
  levelView.draw();
  tankView.draw();
  //FIXME: colManager.debugDraw()
  cursorView.draw();

  if (game.isMovingTank()) {
    const Vector2 touchPoint = game.getTankMoveTouchPoint();
    const Vector2 cursorSize = Vector2(1.0f, 1.0f);
    const Vector2 cursorCenter = touchPoint + cursorSize/2.0f;
    Vector2 dir = cursorCenter-tankView.getCenter();
    dir.normalize();

    //FIXME: paint line

    //Dot product is e [0,pi], so we multiply by relative orientation of the vectors
    float angle = acos(dir*Vector2::X_AXIS) * Vector2::X_AXIS.relativeOrientation(dir);
    arrowEnd.draw(touchPoint, cursorSize, angle);

  }
}
