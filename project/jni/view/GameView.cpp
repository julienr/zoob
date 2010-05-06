#include "GameView.h"
#include "view/GLW.h"
#include "view/Square.h"
#include "logic/physics/Grid.h"
#include "EnemyTankView.h"
#include <logic/Mine.h>

GameView::GameView (const Game& g)
  : game(g),
    playerTankView(g.getPlayerTank()),
    //cursorView(g.getCursor()),
    levelView(g.getLevel()),
    arrowEnd("assets/sprites/arrow_end.png"),
    rocket("assets/sprites/rocket.png"),
    mine("assets/sprites/mine.png"),
    hearthEmpty("assets/sprites/hearth_empty.png"),
    hearthFull("assets/sprites/hearth_full.png"),
    enemiesView(5){
  numbers[0] = new Sprite("assets/sprites/menuitems/0.png");
  numbers[1] = new Sprite("assets/sprites/menuitems/1.png");
  numbers[2] = new Sprite("assets/sprites/menuitems/2.png");
  numbers[3] = new Sprite("assets/sprites/menuitems/3.png");
  numbers[4] = new Sprite("assets/sprites/menuitems/4.png");
  numbers[5] = new Sprite("assets/sprites/menuitems/5.png");
  
  const list<EnemyTank*>* enemies = g.getEnemies();
  for (list<EnemyTank*>::const_iterator i = enemies->begin(); i.hasNext(); i++)
    enemiesView.append(new EnemyTankView(**i));
}

GameView::~GameView () {
  for (size_t i=0; i<enemiesView.length(); i++)
    delete enemiesView[i];
  for (list<Explosion*>::iterator i = explosions.begin(); i.hasNext(); i++)
    delete *i;
}

void GameView::drawHearts () {
  const int currentLife = game.getPlayerNumLives();
  for (int i=0; i<game.getPlayerMaxLives(); i++) {
    if (i >= currentLife)
      hearthEmpty.draw(Vector2(i,0), Vector2(1,1));
    else
      hearthFull.draw(Vector2(i,0), Vector2(1,1));
  }
}

void GameView::draw () {
  //Create new explosions
  for (list<Vector2>::const_iterator i = game.getExplosions(); i.hasNext(); i++) {
    explosions.append(new Explosion(*i));
  }


  levelView.draw();
  
  //mines
  for (list<Mine*>::const_iterator i = game.getMines(); i.hasNext(); i++) {
    Mine* m = *i;
    
    GLW::color(TankView::getColor(m->getOwner()->getTankType()));
    mine.draw(*m);
    
    const int timeLeft = (int)floor(m->getTimeLeft());
    ASSERT(timeLeft < 6);
    numbers[timeLeft]->draw(m->getPosition(), Vector2(1.5,1.5));
    
    GLW::colorWhite();
  }
  
  playerTankView.draw();
  for (size_t i=0; i<enemiesView.length(); i++)
    enemiesView[i]->draw();
  //FIXME: colManager.debugDraw()


  //rockets
  for (list<Rocket*>::const_iterator i = game.getRockets(); i.hasNext(); i++) {
    Rocket* r = *i;
    GLW::color(TankView::getColor(r->getOwner()->getTankType()));
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

  /*if (game.isMovingCursor())
    cursorView.draw();

  if (game.isMovingTank()) {
    const Vector2 touchPoint = game.getTankMoveTouchPoint();
    const Vector2 cursorSize = Vector2(1.0f, 1.0f);
    Vector2 dir = game.getTankMoveDir();
    dir.normalize();


    //FIXME: paint line

    //Dot product is e [0,pi], so we multiply by relative orientation of the vectors
    float angle = acos(dir*Vector2::X_AXIS) * Vector2::X_AXIS.relativeOrientation(dir);
    arrowEnd.draw(touchPoint, cursorSize, angle);
  }*/
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
    box->getCorners(e->getPosition(), corners);
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
    GLW::translate(e->getPosition().x, e->getPosition().y, 0);
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
