#include "GameView.h"
#include "view/GLW.h"
#include "view/Square.h"
#include "logic/physics/Grid.h"
#include "EnemyTankView.h"
#include <logic/Bomb.h>
#include "logic/ShadowPolygon.h"
#include "view/ShadowPolygonView.h"
#include "app.h"
#include "NumberView.h"
#include "view/GameManager.h"


GameView::GameView ()
  : playerTankView(Game::getInstance()->getPlayerTank()),
    //cursorView(g.getCursor()),
    levelView(Game::getInstance()->getLevel()),
    rocket("assets/sprites/rocket.png", TEX_GROUP_GAME),
    bomb("assets/sprites/bomb.png", TEX_GROUP_GAME),
    hearthEmpty("assets/sprites/hearth_empty.png", TEX_GROUP_GAME),
    hearthFull("assets/sprites/hearth_full.png", TEX_GROUP_GAME),
    circle("assets/sprites/circle.png", TEX_GROUP_GAME),
    enemiesView(5),
    shadow("assets/sprites/shadow.png", TEX_GROUP_GAME),
    light("assets/sprites/light.png", TEX_GROUP_GAME),
    levelTxt("assets/sprites/level.png", TEX_GROUP_GAME),
    wtf("assets/sprites/wtf.png", TEX_GROUP_GAME),
    lastLightToggle(BOSS_INTRO_TIME),
    lightOn(true) {
  const list<EnemyTank*>* enemies = Game::getInstance()->getEnemies();
  for (list<EnemyTank*>::const_iterator i = enemies->begin(); i.hasNext(); i++)
    enemiesView.append(new EnemyTankView(*i));
}

GameView::~GameView () {
  for (size_t i=0; i<enemiesView.length(); i++)
    delete enemiesView[i];
  for (list<Explosion*>::iterator i = explosions.begin(); i.hasNext(); i++)
    delete *i;
}

void GameView::drawHearts () const {
  const unsigned currentLife = Game::getInstance()->getPlayerTank()->getLivesLeft();
  for (unsigned i=0; i<Game::getInstance()->getPlayerTank()->getMaxLives(); i++) {
    if (i >= currentLife)
      hearthEmpty.draw(Vector2(i,0), Vector2(1,1));
    else
      hearthFull.draw(Vector2(i,0), Vector2(1,1));
  }
}

void GameView::drawLevelIndicator () const {
  levelTxt.draw(Vector2(1.3,0), Vector2(2, 1));
  NumberView::getInstance()->drawInt(GameManager::getInstance()->getCurrentLevel(), Vector2(3,0.05), Vector2(1.5,1.5));
}

void GameView::_drawLighting() const {
  //Use scissor so shadows are clipped to the level area
   glEnable(GL_SCISSOR_TEST);
   glScissor(XGS(0), YGS(0), Game::getInstance()->getLevel()->getWidth() / xScreenToGame,
                             Game::getInstance()->getLevel()->getHeight() / yScreenToGame);

   //Lighting
   const int numSubdiv = 20;
   const int numVerts = numSubdiv + 2;
   const float angleIncr = 2.0f * M_PI / (float) numSubdiv;
   //FIXME: this is somehow ugly, but this is to cover the whole level
   const float radius = 15.0f;
   //FIXME: make the lighting circle static (like Square) and simply move it around to foolow player's tank
   MGL_DATATYPE lightingVerts[3 * numVerts];
   MGL_DATATYPE lightingCoords[2 * numVerts];
   const Vector2& center = Game::getInstance()->getPlayerTank()->getPosition();
   lightingVerts[0] = fX(center.x);
   lightingVerts[1] = fX(center.y);
   lightingVerts[2] = 0;
   lightingCoords[0] = lightingCoords[1] = 0;
   float angle = 0;
   for (int i = 1; i < numVerts; i++, angle += angleIncr) {
     const Vector2 pos = center + Vector2(radius * cosf(angle), radius * sinf(
         angle));
     lightingVerts[3 * i] = fX(pos.x);
     lightingVerts[3 * i + 1] = fX(pos.y);
     lightingVerts[3 * i + 2] = 0;

     lightingCoords[2 * i] = fX(1);
     lightingCoords[2 * i + 1] = 0;
   }

   light.bind();
   glVertexPointer(3, MGL_TYPE, 0, lightingVerts);
   glTexCoordPointer(2, MGL_TYPE, 0, lightingCoords);
   glDrawArrays(GL_TRIANGLE_FAN, 0, numVerts);
   glDisable(GL_SCISSOR_TEST);
}

void GameView::_drawShadows() const {
  const vector<ShadowPolygon*>& shadows = Game::getInstance()->getPlayerShadows();
  if (shadows.length() != 0) {
    //Use scissor so shadows are clipped to the level area
    glEnable(GL_SCISSOR_TEST);
    glScissor(XGS(0), YGS(0), Game::getInstance()->getLevel()->getWidth() / xScreenToGame,
                              Game::getInstance()->getLevel()->getHeight() / yScreenToGame);
    //Shadows
    shadow.bind();
    for (unsigned i = 0; i < shadows.length(); i++)
      ShadowPolygonView::draw(shadows[i]);
    glDisable(GL_SCISSOR_TEST);
  }
}

#define INTRO_LIGHT_TOGGLE_TIME 0.5f
void GameView::_drawBossIntro () {
  levelView.drawBackground();
  const double timeLeft = Game::getInstance()->getIntroTimeLeft();
  if (lastLightToggle - timeLeft> INTRO_LIGHT_TOGGLE_TIME) {
    lastLightToggle = timeLeft;
    lightOn = !lightOn;
  }

  _drawLighting();
  levelView.drawWalls();
  playerTankView.draw(Game::getInstance()->getLastFrameElapsed());
  wtf.draw(Game::getInstance()->getPlayerTank()->getPosition()+Vector2(0.5,-0.5), Vector2(1,1));

  GLW::colorWhite();
  if (!lightOn)
    _drawShadows();

}

void GameView::draw() {
 if (Game::getInstance()->inIntro())
   _drawBossIntro();
 else {
   _drawGame();
 }
}

void GameView::_drawGame () {
  Game* game = Game::getInstance();
  //Create new explosions
  list<ExplosionLocation>& gameExpls = game->getExplosions();
  while (!gameExpls.empty()) {
    explosions.append(new Explosion(gameExpls.firstElement()));
    gameExpls.removeFirst();
  }

  levelView.drawBackground();

  _drawLighting();

  levelView.drawWalls();

  //bombs radius
  for (list<Bomb*>::const_iterator i = game->getMines(); i.hasNext(); i++) {
    Bomb* m = *i;
    GLW::color(BLACK,0.1f);
    circle.draw(m->getPosition(), Vector2(BOMB_EXPLOSION_RADIUS, BOMB_EXPLOSION_RADIUS));
    GLW::colorWhite();
  }
  
  //bombs
  for (list<Bomb*>::const_iterator i = game->getMines(); i.hasNext(); i++) {
    Bomb* m = *i;
    const int timeLeft = (int)floor(m->getTimeLeft());
    if (timeLeft == 0)
      GLW::color(RED);
    else
      GLW::color(TankView::getColor(m->getOwner()->getTankType()));

    bomb.draw(*m);
    GLW::colorWhite();

    NumberView::getInstance()->drawInt(timeLeft, m->getPosition()-Vector2(0.05f,0), Vector2(0.9,0.9));
  }
  
  playerTankView.draw(game->getLastFrameElapsed());
  for (size_t i=0; i<enemiesView.length(); i++)
    enemiesView[i]->draw();
  //FIXME: colManager.debugDraw()


  //rockets
  for (list<Rocket*>::const_iterator i = game->getRockets(); i.hasNext(); i++) {
    Rocket* r = *i;
    GLW::color(TankView::getColor(r->getOwner()->getTankType()));
    const int numBounces = r->getNumBounces();
    rocket.draw(*r, 0, 0.7f+numBounces*0.3f);
    GLW::colorWhite();
  }

  //FIXME: re-enable
  if (game->getLevel()->hasShadows())
    _drawShadows();
  //FIXME: shadows should be gray

  //Manage explosions life
  for (list<Explosion*>::iterator i = explosions.begin(); i.hasNext();) {
    Explosion* e = *i;
    e->think(game->getLastFrameElapsed());
    if (e->getTimeLeft() <= 0) {
      delete *i;
      i = explosions.remove(i);
    } else {
      e->draw();
      i++;
    }
  }

  /*if (game->isMovingCursor())
    cursorView.draw();

  if (game->isMovingTank()) {
    const Vector2 touchPoint = game->getTankMoveTouchPoint();
    const Vector2 cursorSize = Vector2(1.0f, 1.0f);
    Vector2 dir = game->getTankMoveDir();
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

void GameView::debugVisibility () {
  GLW::disableTextures();
  const VisibilityGrid& vg = Game::getInstance()->getPlayerVisibility();
  const float cs = vg.getCellSize();
  glPushMatrix();
  GLW::translate(-(1 - cs) / 2.0f, -(1 - cs) / 2.0f, 0);
  for (int x = 0; x < vg.getWidth(); x++) {
    for (int y = 0; y < vg.getHeight(); y++) {
      if (!vg.isWalkable(x, y)) {
        glColor4f(0, 0, 0, 0.5f);
      } else {
        switch (vg.getVisibility(x,y)) {
          case VISIBLE:
            glColor4f(1,0,0,0.7f);
            break;
          case HIDDEN:
            glColor4f(0,0,1,0.7f);
            break;
          case PENUMBRA:
            glColor4f(0,1,0,0.7f);
            break;
        }
      }
      glPushMatrix();
      GLW::scale(cs, cs, 1);
      GLW::translate(x, y, 0);
      Square::draw(false);
      glPopMatrix();
    }
  }
  glPopMatrix();
  glColor4f(1, 1, 1, 1);
  GLW::enableTextures();
}

void drawGrid (const Grid& g) {
  const float cs = g.getCellSize();
  glPushMatrix();
  GLW::translate(-(1-cs)/2.0f, -(1-cs)/2.0f,0); 
  for (unsigned x=0; x<g.getWidth(); x++) {
    for (unsigned y=0; y<g.getHeight(); y++) {
      const bool swapC = x%2==y%2;
      glColor4f(swapC?1:0,0,!swapC?1:0,g.dbg_touched(x,y)?0.7f:0.2f);
      glPushMatrix();
      GLW::scale(cs,cs,1);
      GLW::translate(x, y, 0);
      Square::draw(false);
      glPopMatrix();
    }
  }
  glPopMatrix();
  glColor4f(1,1,1,1);
}


void GameView::debugAI () {
  const float cs = Game::getInstance()->getColManager().getGrid().getCellSize();

  //Clearance
  const NumberView* numberView = NumberView::getInstance();
  const AStar* astar = Game::getInstance()->getAStar();
  glPushMatrix();
  GLW::translate(-(1 - cs) / 2.0f, -(1 - cs) / 2.0f, 0);

  for (int x = 0; x < astar->getWidth(); x++) {
    for (int y = 0; y < astar->getHeight(); y++) {
      glPushMatrix();
      GLW::scale(cs, cs, 1);
      GLW::translate(x, y, 0);
      const int clearance = astar->getClearance(x,y);
      if (clearance != -1)
        numberView->drawInt(clearance, Vector2(0, 0), Vector2(1, 1));
      glPopMatrix();
    }
  }

  GLW::disableTextures();

  //overlays
  list<CellOverlay>& overlays = Game::getInstance()->dbg_getCellOverlays();
  LIST_FOREACH(CellOverlay, overlays, o) {
    GLW::color((*o).color, 0.5f);
    glPushMatrix();
    GLW::scale(cs,cs,1);
    GLW::translate((*o).x, (*o).y, 0);
    Square::draw(false);
    glPopMatrix();
  }
  overlays.clear();
  glPopMatrix();

  //paths
  list<DebugPath*>& paths = Game::getInstance()->dbg_getDebugPaths();
  LIST_FOREACH(DebugPath*, paths, p) {
    DebugPath* dp = *p;
    GLW::color(dp->color, 0.7f);
    const Path* path = dp->path;
    for (size_t i=0; i<path->length(); i++) {
      const Vector2& v = path->get(i);
      glPushMatrix();
      GLW::translate(v.x, v.y, 0);
      GLW::scale(0.2f, 0.2f, 1);
      Square::draw(false);
      glPopMatrix();
    }
    delete dp;
  }
  paths.clear();

  glColor4f(1,1,1,1);
  GLW::enableTextures();
}

void GameView::debugShadows () {
  GLW::disableTextures();
  const vector<ShadowPolygon*>& shadows = Game::getInstance()->getPlayerShadows();
  for (unsigned i=0; i<shadows.length(); i++)
    ShadowPolygonView::debugDraw(shadows[i]);
  GLW::enableTextures();
}

void GameView::debugCollisions () {
  GLW::disableTextures();
  Game::getInstance()->getColManager().foreachEntity(drawColEntity);
  //Draw collision normal
  /*
    const Tank* tank = game->getPlayerTank();
    if (tank.collided) {
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
  }*/
  drawGrid(Game::getInstance()->getColManager().getGrid());
  Game::getInstance()->getColManager().getGrid().debugDraw();

  GLW::enableTextures();
}
