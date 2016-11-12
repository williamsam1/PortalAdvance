#ifndef MAIN_H
#define MAIN_H

#include "myLib.h"

#define DEBUG 0
#define COLLISIONLIMIT 30

void startScreen();
void level1Code();
void level2Code();
void level3Code();
void level4Code();
void level5Code();
void level6Code();
void level7Code();
void update();
void draw();
void onRightHeld();
void onLeftHeld();
void onUpHeld();
void onAPressed();
void onSelectPressed();
Tile* createLevel(const int level[]);
Portal newPortal(Tile *tile, int vertical, int topLeft, PortalType type);
Tile* getTileAt(int row, int col);
Tile* getOtherTile(Tile *tile, int vertical);
int areColliding(Image i, Tile t);
int insidePortal(Image *i, Portal *p);
Vector portalVelocity(double rowVel, double colVel, Portal *in, Portal *out);
int isColliding(Image *i);
void resolveCollisions();
void callButtons();
int hasGround(Image *i);
int hasLeftWall(Image *i);
int hasRightWall(Image *i);
Vector fakePortalPosition(Image *i, Portal *in, Portal *out);
int insideTile(Image *i, Tile *t);
int touchingTile(Image *i, Tile *t);
void openExit();
void closeExit();
void elevatorAnim(Tile *nextLevel, int cubeFinalRow);
void exitAnim();

#endif
