#include "zoobdef.h"
#include "GameView.h"
#include "view/GLW.h"
#include "view/primitives/Square.h"
#include "view/primitives/Circle.h"
#include "logic/physics/Grid.h"
#include "EnemyTankView.h"
#include <logic/Bomb.h>
#include "logic/ShadowPolygon.h"
#include "view/ShadowPolygonView.h"
#include "app.h"
#include "NumberView.h"
#include "view/GameManager.h"
#include "lib/FileManager.h"

#include "FontHelper.h"


GameView::GameView ()
  : //cursorView(g.getCursor()),
    levelView(Game::getInstance()->getLevel()),
    rocket("assets/sprites/rocket.png", TEX_GROUP_GAME),
    bomb("assets/sprites/bomb.png", TEX_GROUP_GAME),
    hearthEmpty("assets/sprites/hearth_empty.png", TEX_GROUP_GAME),
    hearthFull("assets/sprites/hearth_full.png", TEX_GROUP_GAME),
    circle("assets/sprites/circle.png", TEX_GROUP_GAME),
    shadow("assets/sprites/shadow.png", TEX_GROUP_GAME),
    light("assets/sprites/light.png", TEX_GROUP_GAME),
    levelTxt("assets/sprites/level.png", TEX_GROUP_GAME),
    wtf("assets/sprites/wtf.png", TEX_GROUP_GAME),
    lastLightToggle(BOSS_INTRO_TIME),
    lightOn(true) {
  Game::getInstance()->attach(this);

  //Load font
  File* fontFile = FileManager::getInstance()->openFile("assets/fonts/OogieBoogie.ttf");
  size_t size;
  const char* buffer = fontFile->readToBuffer(&size);


  fontlib::FTLib* lib = fontlib::FTLib::getInstance();
  font = lib->loadMemoryFont(buffer, size, 30);

  delete [] buffer;
  delete fontFile;

}

GameView::~GameView () {
  for (map<Tank*, TankView*>::iterator i = tankViews.begin(); i.hasNext(); i++) {
    delete i.value();
  }

  delete font;

  for (list<Explosion*>::iterator i = explosions.begin(); i.hasNext(); i++)
    delete *i;
}

void GameView::drawHearts () const {
  PlayerTank* playerTank = Game::getInstance()->getPlayerTank();
  if (!playerTank)
    return;
  const unsigned currentLife = playerTank->getLivesLeft();
  for (unsigned i=0; i<playerTank->getMaxLives(); i++) {
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
  PlayerTank* playerTank = Game::getInstance()->getPlayerTank();
  Vector2 center(7.5, 5);
  if (playerTank)
    center = playerTank->getPosition();
  light.bind();
  glPushMatrix();
  GLW::translate(center);
  GLW::scale(30,30,1);
  Circle::draw(true);
  glPopMatrix();
}

void GameView::_drawShadows() const {
  const vector<ShadowPolygon*>& shadows = Game::getInstance()->getPlayerShadows();
  if (shadows.length() != 0) {
    //Use scissor so shadows are clipped to the level area
    glEnable(GL_SCISSOR_TEST);
    glScissor(getApp()->XGS(0), getApp()->YGS(0), Game::getInstance()->getLevel()->getWidth() / getApp()->getXScreenToGame(),
                              Game::getInstance()->getLevel()->getHeight() / getApp()->getYScreenToGame());
    //Shadows
    shadow.bind();
    for (unsigned i = 0; i < shadows.length(); i++)
      ShadowPolygonView::draw(shadows[i]);
    glDisable(GL_SCISSOR_TEST);
  }
}

#define INTRO_LIGHT_TOGGLE_TIME 0.5f
void GameView::_drawBossIntro (double /*elapsedS*/) {
  levelView.drawBackground();
  const double timeLeft = Game::getInstance()->getIntroTimeLeft();
  if (lastLightToggle - timeLeft> INTRO_LIGHT_TOGGLE_TIME) {
    lastLightToggle = timeLeft;
    lightOn = !lightOn;
  }

  _drawLighting();
  levelView.drawWalls();
  PlayerTank* playerTank = Game::getInstance()->getPlayerTank();
  if (playerTank)
    wtf.draw(playerTank->getPosition()+Vector2(0.5,-0.5), Vector2(1,1));

  GLW::colorWhite();
  if (!lightOn)
    _drawShadows();

}

void GameView::draw(double elapsedS) {
 if (Game::getInstance()->inIntro())
   _drawBossIntro(elapsedS);
 else { 
   _drawGame(elapsedS);
 }
}

void GameView::_drawGame (double elapsedS) {
  Game* game = Game::getInstance();

  levelView.drawBackground();

  //Enable scissor for lighting rendering
  glEnable(GL_SCISSOR_TEST);
  glScissor(getApp()->XGS(0), getApp()->YGS(0), 
            Game::getInstance()->getLevel()->getWidth() / getApp()->getXScreenToGame(),
            Game::getInstance()->getLevel()->getHeight() / getApp()->getYScreenToGame());

  _drawLighting();

  //Manage explosions life and draw explosions lighting
  for (list<Explosion*>::iterator i = explosions.begin(); i.hasNext();) {
    Explosion* e = *i;
    e->think(elapsedS);
    if (e->getTimeLeft() <= 0) {
      delete *i;
      i = explosions.remove(i);
    } else {
      e->drawLighting();
      i++;
    }
  }

  glDisable(GL_SCISSOR_TEST);

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

  //draw tanks
  for (map<Tank*, TankView*>::iterator i = tankViews.begin(); i.hasNext(); i++) {
    const Tank* t = i.key();
    TankView* tv = i.value();
    const Vector2& tp = t->getPosition();
    if (t->isAlive()) {
      tv->draw();

      const char* playerName = game->getPlayerName(t->getID());
      if (playerName) {
        GLW::color(BLACK);
        fontlib::FontHelper::drawCenteredAt(font, game->getPlayerName(t->getID()), 0.5, tp.x, tp.y);
        GLW::color(WHITE);
      }
    }
  }

  //rockets
  for (list<Rocket*>::const_iterator i = game->getRockets(); i.hasNext(); i++) {
    Rocket* r = *i;
    GLW::color(TankView::getColor(r->getOwner()->getTankType()));
    const int numBounces = r->getNumBounces();
    rocket.draw(*r, 0, 0.7f+numBounces*0.3f);
    GLW::colorWhite();
  }

  if (game->getLevel()->hasShadows())
    _drawShadows();

  //Draw explosions
  for (list<Explosion*>::iterator i = explosions.begin(); i.hasNext();i++) {
    Explosion* e = *i;
    e->draw();
  }
}

void GameView::tankAdded (Tank* t) {
  switch (t->getTankCategory()) {
    case CAT_AI:
      tankViews.insert(t, new EnemyTankView(static_cast<const EnemyTank*>(t)));
      break;
    case CAT_PLAYER:
      tankViews.insert(t, new PlayerTankView(static_cast<const PlayerTank*>(t)));
      break;
    case CAT_NET:
      tankViews.insert(t, new PlayerTankView(static_cast<const PlayerTank*>(t)));
      break;
  }
}

void GameView::tankRemoved (Tank* t) {
  tankViews.remove(t);
}

void GameView::addExplosion (const ExplosionInfo& expl) {
  explosions.append(new Explosion(expl));
}

void GameView::drawAABBox (const AABBox* box, const Vector2& position) {
  MGL_DATATYPE verts[12];
  Vector2 corners[4];
  box->getCorners(position, corners);
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
}

void GameView::drawBCircle (const BCircle* circle, const Vector2& position) {
  const unsigned numVerts = 10;
  MGL_DATATYPE verts[numVerts*3];

  for (unsigned i=0; i<numVerts; i++) {
    const float angle = i*2*M_PI/(float)numVerts;
    const Vector2 v(circle->getRadius()*Math::cos(angle), circle->getRadius()*Math::sin(angle));
    verts[3*i] = fX(v.x);
    verts[3*i+1] = fX(v.y);
    verts[3*i+2] = 0;
  }
  glPushMatrix();
  GLW::translate(position.x, position.y, 0);
  glVertexPointer(3, MGL_TYPE, 0, verts);
  glDrawArrays(GL_LINE_LOOP, 0, numVerts);
  glPopMatrix();
}

void drawColEntity (Entity* e) {
  if (e->collided)
    glColor4f(1,0,0,1);
  else
    glColor4f(0,1,0,1);

  if (e->getBVolume()->getType() == TYPE_AABBOX) {
    const AABBox* box = static_cast<const AABBox*>(e->getBVolume());
    GameView::drawAABBox(box, e->getPosition());
  } else { //BCircle
    const BCircle* circle = static_cast<const BCircle*>(e->getBVolume());
    GameView::drawBCircle(circle, e->getPosition());
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

void GameView::debugOverlays () {
#ifdef DEBUG
  const float cs = Game::getInstance()->getColManager().getGrid().getCellSize();
  glPushMatrix();
  GLW::translate(-(1 - cs) / 2.0f, -(1 - cs) / 2.0f, 0);
  GLW::disableTextures();

  //overlays
  const list<CellOverlay>& overlays = Game::getInstance()->dbg_getCellOverlays();
  LIST_FOREACH_CONST(CellOverlay, overlays, o) {
    GLW::color((*o).color, 0.5f);
    glPushMatrix();
    GLW::scale(cs, cs, 1);
    GLW::translate((*o).x, (*o).y, 0);
    Square::draw(false);
    glPopMatrix();
  }
  glPopMatrix();

  //polygons
  const list<DebugPolygon>& polys = Game::getInstance()->dbg_getDebugPolygons();
  LIST_FOREACH_CONST(DebugPolygon, polys, p) {
    const DebugPolygon& poly = *p;
    glPushMatrix();
    GLW::color(poly.color, 0.7f);
    glVertexPointer(2, MGL_TYPE, 0, poly.poly.getVerts());
    glDrawArrays(GL_TRIANGLE_FAN, 0, poly.poly.getNumVerts());
    glPopMatrix();
  }

  glColor4f(1, 1, 1, 1);
  GLW::enableTextures();
#else
  LOGE("debugOverlays: DEBUG flag not turned on");
#endif
}


void GameView::debugAI () {
  const Grid& grid = Game::getInstance()->getColManager().getGrid();
  const float cs = grid.getCellSize();

  //Clearance
  const NumberView* numberView = NumberView::getInstance();
  const PathFinder* pathFinder = Game::getInstance()->getPathFinder();
  glPushMatrix();
  GLW::translate(-(1 - cs) / 2.0f, -(1 - cs) / 2.0f, 0);

  for (unsigned x = 0; x < grid.getWidth(); x++) {
    for (unsigned y = 0; y < grid.getHeight(); y++) {
      glPushMatrix();
      GLW::scale(cs, cs, 1);
      GLW::translate(x, y, 0);
      const int clearance = pathFinder->getClearance(x,y);
      if (clearance != -1)
        numberView->drawInt(clearance, Vector2(0, 0), Vector2(1, 1));
      glPopMatrix();
    }
  }

#ifdef DEBUG
  //paths
  GLW::disableTextures();
  const list<DebugPath*>& paths = Game::getInstance()->dbg_getDebugPaths();
  LIST_FOREACH_CONST(DebugPath*, paths, p) {
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
  }
  GLW::enableTextures();
#endif

  glPopMatrix();
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
