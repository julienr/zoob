#include "levelgen.h"
#include "containers/pair.h"
#include "containers/vector.h"

char tile2char (eTileType t) {
  switch (t) {
    case E: return 'E';
    case W: return 'W';
    case T: return 'T';
    case L: return 'L';
    case B: return 'B';
    case R: return 'R';
    case M: return 'M';
  }
  ASSERT(false);
  return ' ';
}

struct levelData_t {
  levelData_t (int w, int h)
    : width(w), height(h) {
    data = new eTileType*[width];
    for (int i=0; i<width; i++)
      data[i] = new eTileType[height];
  }

  ~levelData_t () {
    for (int i=0; i<width; i++)
      delete [] data[i];
    delete [] data;
   }

  eTileType* operator [] (int idx) {
    return data[idx];
  }

  const eTileType* operator [] (int idx) const {
    return data[idx];
  }

  bool inside (int x, int y) const {
    return x >=0 && x < width && y >= 0 && y < height;
  }

  bool hasNeigh (int x, int y, int tileTypeMask) const {
    for (int i=-1; i<=1; i++) {
      for (int j=-1; j<=1; j++) {
        const int nx = x+i;
        const int ny = y+j;
        if (nx == x && ny == y)
          continue;
        if (!inside(nx,ny))
          continue;
        if (data[nx][ny] & tileTypeMask)
          return true;
      }
    }
    return false;
  }

  const int width;
  const int height;
  private:
  eTileType** data;
};

//fill the borders and the rest with empty tiles
void fillWithEmpty (levelData_t& levelData) {
  const int width = levelData.width;
  const int height = levelData.height;
  for (int x=0; x<width; x++) {
    for (int y=0; y<height; y++) {
      if ((x==0 && y==0) ||
          (x==0 && y==height-1) ||
          (x==width-1 && y==0) ||
          (x==width-1 && y==height-1)) 
        levelData[x][y] = W;
      else if (x==0)
        levelData[x][y] = L;
      else if (x==width-1)
        levelData[x][y] = R;
      else if (y==0)
        levelData[x][y] = T;
      else if (y==height-1)
        levelData[x][y] = B;
      else
        levelData[x][y] = E; 
    }
  }
}

template<class T>
static void swap (vector<T>& arr, size_t x, size_t y) {
  T tmp = arr[x];
  arr[x] = arr[y];
  arr[y] = tmp;
}

//in-place shuffle
template<class T>
static void shuffle (vector<T>& arr) {
  size_t indexes[arr.size()]; //the indexes left to shuffle
  for (size_t i=0; i<arr.size(); i++)
    indexes[i] = i;
  
  int numLeft = arr.size();
  int pos=0;
  while (numLeft > 1) {
    pos = (pos+rand())%numLeft;
    swap(arr, indexes[pos], 0);
    indexes[pos] = indexes[numLeft-1]; //"remove" this index by putting it at the end and "popping" (numLeft--)
    numLeft--;
  }
}

//BEGIN: articulation points

struct Vertex {
  int x, y;
  bool visited;
  int num;
  int low;
  Vertex* parent; //NULL for root
};

static const int neighs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};

//#define ART_LOG(...) printf(__VA_ARGS__)
#define ART_LOG(...) 

class ArtPts {
  public:
    ArtPts (const levelData_t& levelData, vector<pair<int, int> >& artList) 
      : artsList(artList), levelData(levelData), width(levelData.width), height(levelData.height) {
      graph = new Vertex*[width];
      for (int x=0; x<width; x++) {
        graph[x] = new Vertex[height];
        for (int y=0; y<height; y++) {
          graph[x][y].x = x;
          graph[x][y].y = y;
          graph[x][y].visited = false;
          graph[x][y].parent = NULL;
        }
      }
      run();
    }

    ~ArtPts () {
      for (int x=0; x<width; x++)
        delete [] graph[x];
      delete [] graph;
    }

  private:
    void run () {
      //Start with the first empty tile we find
      int startX;
      int startY;
      bool found = false;
      for (startX=0; startX<width; startX++) {
        for (startY=0; startY<height; startY++) {
          if (levelData[startX][startY] == E) {
            found = true;
            break;
          }
        }
        if (found)
          break;
      }
      ASSERT(found);

      printf("starting at [%i,%i]\n", startX, startY);
      counter = 1;
      rootNumChildren = 0;
      findArt(&graph[startX][startY]);
    }

    void findArt (Vertex* v);

    void getAdjs (const Vertex* v, vector<Vertex*>& adjs);

    vector<pair<int, int> >& artsList;

    const levelData_t& levelData;
    const int width;
    const int height;
    Vertex** graph;
    int counter;
    int rootNumChildren;
};

void ArtPts::getAdjs (const Vertex* v, vector<Vertex*>& adjs) {
  for (int i=0; i<4; i++) {
    const int nx = v->x + neighs[i][0];
    const int ny = v->y + neighs[i][1];
    if (!levelData.inside(nx,ny))
      continue;
    adjs.append(&graph[nx][ny]);
  }
}

void ArtPts::findArt (Vertex* v) {
  v->visited = true;
  v->low = v->num = counter++;
  ART_LOG("[art] [%i,%i]\n", v->x, v->y);
  
  vector<Vertex*> adjs(4);
  getAdjs(v, adjs);
  for (size_t i=0; i<adjs.size(); i++) {
    Vertex* neigh = adjs[i];
    if (levelData[neigh->x][neigh->y] != E)
      continue;
    if (!neigh->visited) { //forward edge
      neigh->parent = v;
      findArt(neigh);
      ART_LOG("[pop] [%i,%i]\n", v->x, v->y);
      if (v->parent == NULL) { //root, this is an articulation iff it has more than one child
        rootNumChildren++;
        if (rootNumChildren == 2) {
          artsList.append(pair<int,int>(v->x, v->y));
          ART_LOG("[%i,%i] is an articulation point\n", v->x, v->y);
        }
      } else if (neigh->low >= v->num) {
        //articulation point
        artsList.append(pair<int,int>(v->x, v->y));
        ART_LOG("[%i,%i] is an articulation point\n", v->x, v->y);
      }
      //printf("forward edge from [%i,%i] to [%i,%i]\n", v->x, v->y, neigh->x, neigh->y);
      v->low = MIN(v->low, neigh->low);
      ART_LOG("[%i,%i], low=%i, num=%i\n", v->x, v->y, v->low, v->num);
    } else if (v->parent != neigh) { //back edge
      //printf("back edge from [%i,%i] to [%i,%i]\n", v->x, v->y, neigh->x, neigh->y);
      v->low = MIN(v->low, neigh->num);
    }
  }
}

//END: Articulation points

void printLevel (const levelData_t& levelData, const vector<pair<int, int> >& artPts) {
  printf("num articulations : %i\n", artPts.size());
  char rowBuf[levelData.width*2+1];
  for (int y=0; y<levelData.height; y++) {
    for (int x=0; x<levelData.width; x++) {
      if (artPts.contains(pair<int,int>(x,y)))
        rowBuf[2*x] = 'A';
      else
        rowBuf[2*x] = tile2char(levelData[x][y]);
      rowBuf[2*x+1] = ' ';
    }
    rowBuf[2*levelData.width] = '\0';
    LOGE("%s", rowBuf);
  }
  printf("\n");
}


/** This is done in two passes :
 * 1) randomly select some tile and put a wall on them. The number of selected tile in this step is controlled by the targetSplit.
 *    (width*height)*targetSplit tiles will be put on the first step
 * 2) put new walls near existing until we reach target (width*height)*coverage TOTAL walls (including the first pass ones)
 * EVERY time a new wall is created, check that the map is not splitted (FIXME: use cut vertices concept to do that)
 *
 * targetSplit and targetCoverage are ]0-100[
 */
static void _doFillPass1 (levelData_t& levelData, int targetSplit);
static void _doFillPass2 (levelData_t& levelData, int targetCoverage);

void fillWithWalls (levelData_t& levelData, int targetSplit, int targetCoverage) {
  ASSERT(targetSplit > 0 && targetSplit < 100);
  ASSERT(targetCoverage > 0 && targetCoverage < 100);

  //FIRST PASS
  _doFillPass1(levelData, targetSplit);
  //SECOND PASS
  _doFillPass2(levelData, targetCoverage);
}

static void _doFillPass1 (levelData_t& levelData, int targetSplit) {
  const int numTodo = (levelData.width*levelData.height*targetSplit)/100;
  LOGE("[doFillPass1] numTodo=%i", numTodo);
  int numAssigned = 0;
  vector<pair<int, int> > coords(levelData.width*levelData.height);
  for (int x=0; x<levelData.width; x++) {
    for (int y=0; y<levelData.height; y++) {
      if (levelData[x][y] != E)
        continue;
      coords.append(pair<int, int>(x,y));
    }
  }
  shuffle(coords);

  for (size_t i=0; i<coords.size() && numAssigned<numTodo; i++) {
    const int x = coords[i].first;
    const int y = coords[i].second;
    if (levelData[x][y] != E)
      continue;
    if (levelData.hasNeigh(x,y, M | W | L | T | R | B))
      continue;
    numAssigned++;
    levelData[x][y] = M;
  }
  LOGE("[doFillPass1] finished, %i/%i assigned", numAssigned, numTodo);
}

static void _doFillPass2 (levelData_t& levelData, int targetCoverage) {
  const int numTodo = (levelData.width*levelData.height*targetCoverage)/100;
  LOGE("[doFillPass2] numTodo=%i", numTodo);
  //First, find articulation points. We shouldn't add any wall here
  vector<pair<int, int> > artPts(10); 
  ArtPts art(levelData, artPts);
  printLevel(levelData, artPts);


  int numAssigned = 0;
  vector<pair<int, int> > coords(levelData.width*levelData.height);
  for (int x=0; x<levelData.width; x++) {
    for (int y=0; y<levelData.height; y++) {
      const pair<int,int> point(x,y);
      if (levelData[x][y] != E)
        continue;
      if (artPts.contains(point))
        continue;
      coords.append(point);
    }
  }
  shuffle(coords);

  for (size_t i=0; i<coords.size() && numAssigned < numTodo; i++) {
    const int x = coords[i].first;
    const int y = coords[i].second;
    if (levelData[x][y] != E)
      continue;
    if (artPts.contains(pair<int,int>(x,y)))
      continue;
    if (!levelData.hasNeigh(x, y, M)) 
      continue;
    numAssigned++;
    LOGE("[doFillPass2] adding wall at [%i,%i]\n", x, y);
    levelData[x][y] = M;
    artPts.clear();
    ArtPts art(levelData, artPts);
    printLevel(levelData, artPts);
  }
  LOGE("[doFillPass2] finished, %i/%i assigned", numAssigned, numTodo);
}

#define LD(i) levelData[x+neighs[i][0]][y+neighs[i][1]]
#define NSOLID(i) (LD(i) != E)
#define NE(i) levelData.inside(x+neighs[i][0], y+neighs[i][1])

//give 4 booleans. It returns a condition where all NSOLID shoud
#define NCOND(left,right,bottom,top) ((!NE(0) || NSOLID(0) == left) && \
                                      (!NE(1) || NSOLID(1) == right) && \
                                      (!NE(2) || NSOLID(2) == bottom) && \
                                      (!NE(3) || NSOLID(3) == top))


//neighs : {-1,0}, {1,0}, {0,-1}, {0,1}

//Try to transform the walls so they look the best possible
void transformWalls (levelData_t& levelData) {
  for (int x=0; x<levelData.width; x++) {
    for (int y=0; y<levelData.height; y++) {
      if (levelData[x][y] == E)
        continue;
      if (NCOND(false,false,false,false)) //all neighbours empty
        levelData[x][y] = M;
      else if (NCOND(true,true,false,false)) { //left and right solid
        if (levelData[x][y] == B || levelData[x][y] == T)
          continue;
        levelData[x][y] = T;
      } else if (NCOND(false,false,true,true)) { //top and bottom solid
        if (levelData[x][y] == R || levelData[x][y] == L)
          continue;
        levelData[x][y] = R;
      } else if (NCOND(true,false,false,false))
        levelData[x][y] = LD(0)==M?B:LD(0);
      else if (NCOND(false,true,false,false))
        levelData[x][y] = LD(1)==M?B:LD(1);
      else if (NCOND(false,false,true,false))
        levelData[x][y] = LD(2)==M?R:LD(2);
      else if (NCOND(false,false,false,true))
        levelData[x][y] = LD(3)==M?R:LD(3);
      else
        levelData[x][y] = W;
    }
  }
}

void generateLevel (levelData_t& levelData, int targetSplit, int targetCoverage) {
  fillWithEmpty(levelData);
  fillWithWalls(levelData, targetSplit, targetCoverage);
  transformWalls(levelData);
}

#include <time.h>

//Returns a newly-allocated level, freeing is up to the caller
eTileType* generateLevel (int width, int height) {
  srand(time(NULL));

  levelData_t levelData(width, height);
  generateLevel(levelData, 5, 10);

  eTileType* data = new eTileType[width*height];
  for (int x=0; x<width; x++) 
    for (int y=0; y<height; y++) 
      data[y*width+x] = levelData[x][y];
  return data;
}
