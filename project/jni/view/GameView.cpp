#include "GameView.h"
#include "view/GLW.h"
#include "view/Square.h"
#include "logic/physics/Grid.h"

GameView::GameView (const Game& g)
  : game(g),
    tankView(g.getPlayerTank()),
    cursorView(g.getCursor()),
    levelView(g.getLevel()),
    arrowEnd("assets/sprites/arrow_end.png"),
    rocket("assets/sprites/rocket.png"),
    enemiesView(5) {
  const vector<Tank*> enemies = g.getEnemies();
  for (size_t i=0; i<enemies.length(); i++)
    enemiesView.add(new TankView(*(enemies[i])));
}

GameView::~GameView () {
  for (size_t i=0; i<enemiesView.length(); i++)
    delete enemiesView[i];
}

void GameView::draw () {
  //Create new explosions
  for (list<Vector2>::const_iterator i = game.getExplosions(); i.hasNext(); i++) {
    explosions.append(new Explosion(*i));
  }


  levelView.draw();
  tankView.draw();
  for (size_t i=0; i<enemiesView.length(); i++)
    enemiesView[i]->draw();
  //FIXME: colManager.debugDraw()


  for (list<Rocket*>::const_iterator i = game.getRockets(); i.hasNext(); i++) {
    Rocket* r = *i;
    GLW::color(r->getOwner()->getColor());
    rocket.draw(*r);
    GLW::colorWhite();
  }

  //Manage explosions life
  for (list<Explosion*>::iterator i = explosions.begin(); i.hasNext();) {
    Explosion* e = *i;
    e->think(game.getLastFrameElapsed());
    if (e->getTimeLeft() <= 0)
      i = explosions.remove(i);
    else {
      e->draw();
      i++;
    }
  }

  if (game.isMovingCursor())
    cursorView.draw();

  if (game.isMovingTank()) {
    const Vector2 touchPoint = game.getTankMoveTouchPoint();
    const Vector2 cursorSize = Vector2(1.0f, 1.0f);
    /*const Vector2 cursorCenter = touchPoint + cursorSize/2.0f;
    Vector2 dir = cursorCenter-tankView.getCenter();
    dir.normalize();*/
    Vector2 dir = game.getTankMoveDir();
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

  if (e->getBVolume()->getType() == TYPE_AABBOX) {
    const AABBox* box = static_cast<const AABBox*>(e->getBVolume());
    MGL_DATATYPE verts[12];
    Vector2 corners[4];
    box->getCorners(corners);
    for (int i=0; i<4; i++) {
      verts[3*i] = fX(corners[i].x);
      verts[3*i+1] = fX(corners[i].y);
      verts[3*i+2] = 0;
      //LOGE("corners[%i] : (%f,%f)", i, corners[i].x, corners[i].y);
    }
    glPushMatrix();
    glVertexPointer(3, MGL_TYPE, 0, verts);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glPopMatrix();
  } else { //BCircle
    const BCircle* circle = static_cast<const BCircle*>(e->getBVolume());
    const unsigned numVerts = 10;
    MGL_DATATYPE verts[numVerts*3];

    for (unsigned i=0; i<numVerts; i++) {
      const float angle = i*2*M_PI/(float)numVerts;
      const Vector2 v(circle->getRadius()*cosf(angle), circle->getRadius()*sinf(angle));
      verts[3*i] = fX(v.x);
      verts[3*i+1] = fX(v.y);
      verts[3*i+2] = 0;
    }
    glPushMatrix();
    GLW::translate(circle->getPosition().x, circle->getPosition().y, 0);
    glVertexPointer(3, MGL_TYPE, 0, verts);
    glDrawArrays(GL_LINE_LOOP, 0, numVerts);
    glPopMatrix();
  }
  glColor4f(1,1,1,1);
}

void drawGrid (const Grid& g) {
  const float cs = g.getCellSize();
  for (unsigned x=0; x<g.getWidth(); x++) {
    for (unsigned y=0; y<g.getHeight(); y++) {
      const bool swapC = x%2==y%2;
      glColor4f(swapC?1:0,0,!swapC?1:0,g.touched(x,y)?0.7f:0.2f);
      glPushMatrix();
      GLW::translate(x*cs, y*cs, 0);
      Square::draw(false);
      glPopMatrix();
    }
  }
  glColor4f(1,1,1,1);
}

void GameView::debugDraw () {
  const Tank& tank = game.getPlayerTank();
  glDisable(GL_TEXTURE_2D);
  game.getColManager().foreachEntity(drawColEntity);
  //Draw collision normal
  //if (tank.collided) {
    MGL_DATATYPE verts[6] = {
        fX(tank.lastColNormal.x), fX(tank.lastColNormal.y), 0,
        0, 0, 0
    };
    glPushMatrix();
    GLW::translate(tank.lastColPoint.x, tank.lastColPoint.y, 0);
    glLineWidth(5.0f);
    glVertexPointer(3, MGL_TYPE, 0, verts);
    glDrawArrays(GL_LINES, 0, 2);
    glPopMatrix();
    glLineWidth(1.0f);
  //}
  drawGrid(game.getColManager().getGrid());
  glEnable(GL_TEXTURE_2D);
}
