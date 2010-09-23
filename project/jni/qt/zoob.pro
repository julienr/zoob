######################################################################
# Automatically generated by qmake (2.01a) Fri Sep 3 13:50:40 2010
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ../ /usr/include/GL/
QT += opengl
LIBS += -L/usr/local/lib -lm -lpng -lzip -ljansson
DEFINES += PLATFORM_SDL
#Android doesn't support exceptions, so zoob isn't using them
QMAKE_CXXFLAGS += -fno-rtti -fno-exceptions
#PROFILING
QMAKE_CXXFLAGS_DEBUG += -pg -DDEBUG
QMAKE_LFLAGS_DEBUG += -pg

#FPS debug
debug {
  DEFINES += ZOOB_DBG_FPS
}

QMAKE_CXX = colorgcc

# Input
#qt specific first
HEADERS += GameScreen.h MainWindow.h MenuScreen.h InterMenu.h DebugAction.h
SOURCES += main.cpp MainWindow.cpp MenuScreen.cpp GameScreen.cpp InterMenu.cpp DebugAction.cpp
#zoob (copied from Android.mk, but changed assignation to = instead of :=)
LOCAL_SRC_FILES = \
		app.cpp \
    lib/Vector2.cpp \
    lib/TimerManager.cpp \
    lib/Polygon.cpp \
    lib/Line.cpp \
    logic/Viewable.cpp \
    logic/Level.cpp \
    logic/Game.cpp \
    logic/Tank.cpp \
    logic/ProgressionManager.cpp \
    logic/PlayerTank.cpp \
    logic/ShadowPolygon.cpp \
    levels/LevelsData.cpp \
    levels/levelgen.cpp \
    view/textures/TextureManager.cpp \
    view/textures/TextureGroup.cpp \
    view/textures/Texture.cpp \
    view/Sprite.cpp \
    view/Square.cpp \
    view/LevelView.cpp \
    view/GameView.cpp \
    view/Explosion.cpp \
    view/GameManager.cpp \
    view/NumberView.cpp \
    view/menu/MenuItem.cpp \
    view/menu/Menu.cpp \
    view/menu/PausedMenu.cpp \
    view/menu/Button.cpp \
    view/ShadowPolygonView.cpp \
    logic/physics/AABBox.cpp \
    logic/physics/CollisionManager.cpp \
    logic/physics/Grid.cpp \
    logic/Difficulty.cpp \
    ai/shoot/AimPolicy.cpp \
    ai/shoot/RandomPolicy.cpp \
    ai/shoot/AlwaysPolicy.cpp \
    ai/movement/PathPolicy.cpp \
    ai/movement/SmartPolicy.cpp \
    ai/TankAI.cpp \
    ai/algorithms/AStar.cpp \
    ai/algorithms/PathFinder.cpp \
    logic/VisibilityGrid.cpp \
    input/AndroidInputManager.cpp \
    input/PlayerFormControl.cpp \
    input/InputManager.cpp

for(file,LOCAL_SRC_FILES) {
  SOURCES += ../$$file
}

