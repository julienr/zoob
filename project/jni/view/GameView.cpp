#include "GameView.h"
#include "view/GLW.h"
#include "view/Square.h"

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

void drawColEntity (Entity* e) {
  if (e->collided)
    glColor4f(1,0,0,1);
  else
    glColor4f(0,1,0,1);

  int verts[12];
  Vector2 corners[4];
  e->getBBox().getCorners(corners);
  for (int i=0; i<4; i++) {
    verts[3*i] = fX(corners[i].x);
    verts[3*i+1] = fX(corners[i].y);
    verts[3*i+2] = 0;
    //LOGE("corners[%i] : (%f,%f)", i, corners[i].x, corners[i].y);
  }


  glPushMatrix();
  glVertexPointer(3, GL_FIXED, 0, verts);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
  glPopMatrix();
  glColor4f(1,1,1,1);
}

void GameView::debugDraw () {
  glDisable(GL_TEXTURE_2D);
  game.getColManager().foreachEntity(drawColEntity);
  glEnable(GL_TEXTURE_2D);
}
