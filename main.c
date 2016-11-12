#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "myLib.h"
#include "images/companionCube.h"
#include "levels.h"
#include "images/title.h"
#include "images/door.h"
#include "images/endScreen.h"

Tile *tiles;

Portal *orange = NULL;
Portal *blue = NULL;
Portal p1;
Portal p2;

int startRow = 130;
int startCol = 3*TILE_WIDTH;

double g = 0.5;
double airFriction = 0.01;
double colVel = 0;
double rowVel = 0;

Image cube;
Rectangle cubeShadow;
Image fakeCube;
Rectangle fakeCubeShadow;
int ground = FALSE;
int leftWall = FALSE;
int rightWall = FALSE;
Portal *inPortal = NULL;
Portal *outPortal = NULL;
Portal *lastInPortal = NULL;
Portal *lastOutPortal = NULL;
int drawFakeShadow = FALSE;
Tile *exitTile;

int reverseFramesLeft = 0;

int level = 0;
int state = 0;

volatile int rightDown = 0;
volatile int rightWasReleased = 1;
volatile int leftDown = 0;
volatile int leftWasReleased = 1;
volatile int upDown = 0;
volatile int upWasReleased = 1;
volatile int downDown = 0;
volatile int aDown = 0;
volatile int aWasReleased = 1;
volatile int selectDown = 0;
volatile int selectWasReleased = 1;

int main() {
	//main function, called when game starts
	REG_DISPCNT = MODE_3 | BG2_EN;
	startScreen();
	return 0;
	/*
	tiles = createLevel(level7);
	for (int i = 0; i < 150; i++) {
		if (tiles[i].type != EMPTY) {
			for (int j = 0; j < 10; j++) {
				waitForVblank();
			}
			drawTile(&tiles[i]);
		}
	}
	cube = (Image){3*TILE_HEIGHT - COMPANIONCUBE_HEIGHT, 2 + TILE_WIDTH,
		COMPANIONCUBE_WIDTH, COMPANIONCUBE_HEIGHT, companionCube};
	fakeCube = (Image){cube.row, cube.col, cube.width, cube.height, companionCube};
	level7Code();
	*/
	
}

void startScreen() {
	//displays the start screen and waits for user to press A
	level = 0;
	orange = NULL;
	blue = NULL;
	drawImage3(0, 0, WIDTH, HEIGHT, title);
	while(!aDown) {
		aDown = KEY_DOWN_NOW(BUTTON_A);
	}

	for (int i = 0; i <= HEIGHT / 2; i++) {
		waitForVblank();
		shiftScreenVert(2, BLACK);
	}
	level1Code();
}

void level1Code() {
	//code for controlling level 1
	level = 1;
	state = 0;
	tiles = createLevel(level1);
	for (int i = 0; i < 150; i++) {
		if (tiles[i].type != EMPTY) {
			for (int j = 0; j < 10; j++) {
				waitForVblank();
			}
			drawTile(&tiles[i]);
		}
	}

	cube = (Image){125, 48, COMPANIONCUBE_WIDTH, COMPANIONCUBE_HEIGHT, companionCube};
	fakeCube = (Image){cube.row, cube.col, cube.width, cube.height, companionCube};

	while(!insideTile(&cube, exitTile)) {
		callButtons();
		update();
		waitForVblank();
		if (state == 0) {
			drawString(20, 20, "Hello, and again welcome to the", WHITE);
			drawString(30, 20, "Aperture Science Computer Aided", WHITE);
			drawString(40, 20, "Enrichment Center.", WHITE);
			drawString(50, 20, "(Press A to advance)", WHITE);
			state++;
		} else if (state == 2) {
			drawRect(20, 20, 12*TILE_WIDTH, 50, BLACK);
			drawString(20, 20, "We hope your brief detention in", WHITE);
			drawString(30, 20, "the relaxation vault has been a", WHITE);
			drawString(40, 20, "pleasant one. Your specimen has", WHITE);
			drawString(50, 20, "been processed, and we are now", WHITE);
			drawString(60, 20, "ready to begin the test proper.", WHITE);
			state++;
		} else if (state >= 4 && state <= 184) {
			if (state == 124) {
				drawRect(20, 10*TILE_WIDTH + 5, 2*TILE_WIDTH, 10, BLACK);
				drawString(20, 20, "                         1", WHITE);
			} else if (state == 64) {
				drawRect(20, 10*TILE_WIDTH + 5, 2*TILE_WIDTH, 10, BLACK);
				drawString(20, 20, "                         2", WHITE);
			} else if (state == 4) {
				drawRect(20, 20, 12*TILE_WIDTH, 50, BLACK);
				drawString(20, 20, "The portal will open in: 3", WHITE);
			}
			state++;
		} else if (state == 185) {
			openExit();
			p1 = newPortal(&tiles[OFFSET(7, 5, 15)], TRUE, TRUE, PORTAL_BLUE);
			p2 = newPortal(&tiles[OFFSET(7, 6, 15)], TRUE, FALSE, PORTAL_ORANGE);
			blue = &p1;
			orange = &p2;
			waitForVblank();
			drawRect(20, 20, 12*TILE_WIDTH, 50, BLACK);
			drawPortal(orange);
			drawPortal(blue);
			state++;
		}
		draw();
	}
	tiles = createLevel(level2);
	elevatorAnim(tiles, 8);
	level2Code();
}

void level2Code() {
	//code for controlling level 2
	level = 2;
	state = 0;
	openExit();
	while(!insideTile(&cube, exitTile)) {
		callButtons();
		update();
		if (state == 0) {
			if (orange) {
				drawTile(orange -> tile);
				drawTile(orange -> other);
				drawTile(blue -> tile);
				drawTile(blue -> other);
			}
			p1 = newPortal(&tiles[OFFSET(7, 10, 15)], TRUE, FALSE, PORTAL_BLUE);
			p2 = newPortal(&tiles[OFFSET(7, 5, 15)], TRUE, FALSE, PORTAL_ORANGE);
			blue = &p1;
			orange = &p2;
			drawPortal(orange);
			drawPortal(blue);
		} else if (state == 179) {
			drawTile(orange -> tile);
			drawTile(orange -> other);
			drawTile(blue -> tile);
			drawTile(blue -> other);
			p1 = newPortal(&tiles[OFFSET(7, 9, 15)], TRUE, TRUE, PORTAL_BLUE);
			p2 = newPortal(&tiles[OFFSET(7, 4, 15)], TRUE, TRUE, PORTAL_ORANGE);
			blue = &p1;
			orange = &p2;
			drawPortal(orange);
			drawPortal(blue);
		}
		if (inPortal == NULL) {
			state = (state + 1) % 360;
		}
		waitForVblank();
		draw();
	}
	tiles = createLevel(level3);
	elevatorAnim(tiles, 8);
	level3Code();
}

void level3Code() {
	//code for controlling level 3
	level = 3;
	state = 0;

	while (state <= 5) {
		callButtons();
		waitForVblank();
		if (state == 0) {
			drawString(50, 20, "Please note; momentum, a", WHITE);
			drawString(60, 20, "function of mass and", WHITE);
			drawString(70, 20, "velocity, is conserved", WHITE);
			drawString(80, 20, "between portals.", WHITE);
			state++;
		} else if (state == 2) {
			drawRect(50, 20, 11*TILE_WIDTH - 5, 50, BLACK);
			drawString(50, 20, "In layman's terms;", WHITE);
			drawString(60, 20, "speedy thing goes in,", WHITE);
			drawString(70, 20, "speedy thing comes out.", WHITE);
			state++;
		} else if (state == 4) {
			drawRect(50, 20, 11*TILE_WIDTH - 5, 50, BLACK);
			state++;
		} else if (state == 5) {
			openExit();
			p1 = newPortal(&tiles[OFFSET(1, 5, 15)], TRUE, FALSE, PORTAL_BLUE);
			p2 = newPortal(&tiles[OFFSET(9, 5, 15)], FALSE, TRUE, PORTAL_ORANGE);
			blue = &p1;
			orange = &p2;
			waitForVblank();
			drawPortal(orange);
			drawPortal(blue);
			state++;
		}
	}

	while(!insideTile(&cube, exitTile)) {
		callButtons();
		update();
		waitForVblank();
		draw();
	}
	tiles = createLevel(level4);
	elevatorAnim(tiles, 3);
	level4Code();
}

void level4Code() {
	//code for controlling level 4
	Tile *gunTile = &tiles[OFFSET(8, 6, 15)];
	level = 4;
	state = 0;
	openExit();
	p2 = newPortal(&tiles[OFFSET(0, 11, 15)], FALSE, FALSE, PORTAL_ORANGE);
	orange = &p2;
	waitForVblank();
	drawPortal(blue);
	while(!touchingTile(&cube, gunTile)) {
		callButtons();
		update();
		waitForVblank();
		draw();
	}
	gunTile -> type = EMPTY;
	waitForVblank();
	drawTile(gunTile);
	while (state <= 4) {
		callButtons();
		waitForVblank();
		if (state == 0) {
			drawString(20, 20, "Very good. You are now in", WHITE);
			drawString(30, 20, "possession of the handheld", WHITE);
			drawString(40, 20, "portal device. With it you", WHITE);
			drawString(50, 20, "can create your own portals.", WHITE);
			state++;
		} else if (state == 2) {
			drawRect(TILE_HEIGHT, TILE_WIDTH, 13*TILE_WIDTH, 3*TILE_HEIGHT, BLACK);
			drawString(20, 20, "(Hold movement buttons and",WHITE);
			drawString(30, 20, "press A to fire a portal", WHITE);
			drawString(40, 20, "in that direction.)", WHITE);
			state++;
		} else if (state == 4) {
			drawRect(TILE_HEIGHT, TILE_WIDTH, 13*TILE_WIDTH, 3*TILE_HEIGHT, BLACK);
			state++;
		}
	}
	while(!insideTile(&cube, exitTile)) {
		callButtons();
		update();
		waitForVblank();
		draw();
	}

	tiles = createLevel(level5);
	elevatorAnim(tiles, 8);
	level5Code();
}

void level5Code() {
	//code for controlling level 5
	level = 5;
	state = 0;
	openExit();
	p2 = newPortal(&tiles[OFFSET(4, 9, 15)], TRUE, TRUE, PORTAL_ORANGE);
	orange = &p2;
	waitForVblank();
	drawPortal(orange);
	while(!insideTile(&cube, exitTile)) {
		callButtons();
		update();
		waitForVblank();
		draw();
	}
	tiles = createLevel(level6);
	elevatorAnim(tiles, 8);
	level6Code();
}

void level6Code() {
	//code for controlling level 6
	level = 6;
	state = 0;
	openExit();
	p2 = newPortal(&tiles[OFFSET(1, 1, 15)], TRUE, FALSE, PORTAL_ORANGE);
	orange = &p2;
	waitForVblank();
	drawPortal(orange);
	while(!insideTile(&cube, exitTile)) {
		callButtons();
		update();
		waitForVblank();
		draw();
	}
	tiles = createLevel(level7);
	elevatorAnim(tiles, 8);
	level7Code();
}

void level7Code() {
	//code for controlling level 7
	level = 7;
	state = 0;
	openExit();
	p2 = newPortal(&tiles[OFFSET(0, 7, 15)], FALSE, FALSE, PORTAL_ORANGE);
	orange = &p2;
	while(!insideTile(&cube, exitTile)) {
		callButtons();
		update();
		if (state == 0) {
			drawTile(orange -> tile);
			drawTile(orange -> other);
			p2 = newPortal(&tiles[OFFSET(0, 7, 15)], FALSE, FALSE, PORTAL_ORANGE);
			orange = &p2;
			drawPortal(orange);
		} else if (state == 179) {
			drawTile(orange -> tile);
			drawTile(orange -> other);
			p2 = newPortal(&tiles[OFFSET(1, 0, 15)], TRUE, FALSE, PORTAL_ORANGE);
			orange = &p2;
			drawPortal(orange);
		}
		if (inPortal == NULL) {
			state = (state + 1) % 360;
		}
		waitForVblank();
		draw();
	}
	exitAnim();
}

void update() {
	//Updates the position of the cube

	reverseFramesLeft = reverseFramesLeft ? reverseFramesLeft - 1 : 0;
	cubeShadow = (Rectangle){cube.row, cube.col, cube.width, cube.height, BLACK};

	if (inPortal != NULL && insidePortal(&cube, inPortal)) {
		//check if exited portal last frame
		inPortal = NULL;
	}

	if (inPortal == NULL && lastInPortal != NULL) {
		//exited portal last frame

		//went through vertical portals in same direction, need to
		//provide time for inverted L-R controls
		if (lastInPortal -> vertical && lastOutPortal -> vertical &&
			lastInPortal -> topLeft == lastOutPortal -> topLeft) {
				reverseFramesLeft = 20;
			}
		Vector v = portalVelocity(rowVel, colVel, lastInPortal, lastOutPortal);
		rowVel = v.row;
		colVel = v.col;

		cube.row = fakeCube.row;
		cube.col = fakeCube.col;
		ground = hasGround(&cube);
		leftWall = hasLeftWall(&cube);
		rightWall = hasRightWall(&cube);
	} else {
		//update normally
		cube.row = cube.row + rowVel;
		cube.col = cube.col + colVel;
		resolveCollisions();
		ground = hasGround(&cube);
		leftWall = hasLeftWall(&cube);
		rightWall = hasRightWall(&cube);
		rowVel = ground ? 0 : rowVel + g - airFriction*rowVel;
		colVel = ground ? 0 : (colVel > 0 ?
			(rightWall ? 0 : colVel) : (leftWall ? 0 : colVel));
	}

	if (inPortal != NULL) {
		//in portal this frame
		lastInPortal = inPortal;
		lastOutPortal = outPortal;
		fakeCubeShadow = (Rectangle){fakeCube.row, fakeCube.col,
			fakeCube.width, fakeCube.height, BLACK};

		Vector p = fakePortalPosition(&cube, inPortal, outPortal);
		fakeCube.row = p.row;
		fakeCube.col = p.col;
	}

	//Limit speed (can skip entire block with portal if not limited
	//to go <= the size of a tile in one tick)
	if (rowVel > TILE_HEIGHT) {
		rowVel = TILE_HEIGHT;
	} else if (rowVel < -TILE_HEIGHT) {
		rowVel = -TILE_HEIGHT;
	}

	if (colVel > TILE_WIDTH) {
		colVel = TILE_WIDTH;
	} else if (colVel < -TILE_WIDTH) {
		colVel = -TILE_WIDTH;
	}
}

void draw() {
	//draws the cube, shadow that trails the cube, and any other
	//parts of the screen that need to be redrawn
	if (DEBUG) {
		char a[4];
		drawRect(0, 5, 18, 8, BLACK);
		sprintf(a, "%d", cube.row);
		drawString(0, 5, a, RED);
		drawRect(0, 25, 18, 8, BLACK);
		sprintf(a, "%d", cube.col);
		drawString(0, 25, a, RED);

		drawRect(0, 55, 18, 8, BLACK);
		sprintf(a, "%d", fakeCube.row);
		drawString(0, 55, a, RED);
		drawRect(0, 75, 18, 8, BLACK);
		sprintf(a, "%d", fakeCube.col);
		drawString(0, 75, a, RED);
	}
	drawRectangle(&cubeShadow);
	if (inPortal == NULL && lastInPortal != NULL) {
		//exited portal last frame
		drawTile(lastInPortal -> tile);
		drawTile(lastInPortal -> other);

		drawRectangle(&fakeCubeShadow);
		drawTile(lastOutPortal -> tile);
		drawTile(lastOutPortal -> other);

		drawPortal(lastInPortal);
		drawPortal(lastOutPortal);
		lastInPortal = NULL;
		lastOutPortal = NULL;
	}
	drawImage(&cube);

	if (inPortal != NULL) {
		//in portal
		if (DEBUG) {
			drawRect(10, 0, 5, 5, RED);
		}
		if (drawFakeShadow) {
			drawRectangle(&fakeCubeShadow);
		} else {
			//don't draw shadow for fake cube the first time
			drawFakeShadow = TRUE;
		}
		drawImage(&fakeCube);
		drawTile(inPortal -> tile);
		drawTile(inPortal -> other);

		drawTile(outPortal -> tile);
		drawTile(outPortal -> other);

		drawPortal(inPortal);
		drawPortal(outPortal);
	} else {
		drawPortal(orange);
		drawPortal(blue);
		//not in portal
		if (DEBUG) {
			drawRect(10, 0, 5, 5, BLACK);
		}
		drawFakeShadow = FALSE;
	}

	if (DEBUG) {
		if (leftWall) {
			drawRect(20, 0, 5, 5, RED);
		} else {
			drawRect(20, 0, 5, 5, BLACK);
		}
		if (rightWall) {
			drawRect(20, 5, 5, 5, RED);
		} else {
			drawRect(20, 5, 5, 5, BLACK);
		}
		if (ground) {
			drawRect(25, 0, 10, 5, RED);
		} else {
			drawRect(25, 0, 10, 5, BLACK);
		}
	}
}

Vector portalVelocity(double rowVel, double colVel, Portal *in, Portal *out) {
	//calculates the new velocity of the cube after exiting a portal

	double inVel;
	if (in -> vertical) {
		inVel = colVel;
	} else {
		inVel = rowVel;
	}
	inVel = inVel < 0 ? -inVel : inVel;

	if (out -> vertical) {
		if (out -> topLeft) {
			return (Vector){0, -inVel};
		} else {
			return (Vector){0, inVel};
		}
	} else {
		if (out -> topLeft) {
			return (Vector){-inVel, 0};
		} else {
			return (Vector){inVel, 0};
		}
	}
}

Vector fakePortalPosition(Image *i, Portal *in, Portal *out) {
	//calculates the position of the fake cube that
	//is drawn when the cube is entering a portal
	int inside;
	int dif;

	if (in -> vertical) {
		dif = i -> row -  in -> tile -> row;
		if (in -> topLeft) {
			inside = i -> col + i -> width - in -> tile -> col;
		} else {
			inside = in -> tile -> col + TILE_WIDTH - i -> col;
		}
	} else {
		dif = i -> col - in -> tile -> col;
		if (in -> topLeft) {
			inside = i -> row + i -> height - in -> tile -> row;
		} else {
			inside = in -> tile -> row + TILE_HEIGHT - i -> row;
		}
	}
	dif = dif < 0 ? -dif : dif;
	inside = inside < 0 ? -inside : inside;

	if (out -> vertical) {
		if (out -> topLeft) {
			return (Vector){out -> tile -> row + dif, out -> tile -> col - inside};
		} else {
			return (Vector){out -> tile -> row + dif, out -> tile -> col + TILE_WIDTH
				+ inside - i -> width};
		}
	} else {
		if (out -> topLeft) {
			return (Vector){out -> tile -> row - inside, out -> tile -> col + dif};
		} else {
			return (Vector){out -> tile -> row + TILE_HEIGHT + inside - i -> height,
				out -> tile -> col + dif};
		}
	}
}

void resolveCollisions() {
	//a posteriori collision resolution for the cube

	static int num = 0;
	double maxVel;
	if (rowVel == 0) {
		maxVel = colVel;
	} else if (colVel == 0) {
		maxVel = rowVel;
	} else {
		maxVel = colVel > rowVel ? colVel : rowVel;
	}
	double dt = 1 / maxVel;
	dt = dt > 0 ? dt : -dt;
	num = (num + 1)%10;

	if (DEBUG && num == 0) {
		char b[4];
		drawRect(0, 140, 100, 8, BLACK);
		sprintf(b, "%f", maxVel);
		drawString(0, 140, b, RED);
	}

	double t = 0;
	if (DEBUG) {
		drawRect(0, 0, 5, 5, RED);
	}
	int n = 0;
	while (isColliding(&cube)) {
		if (n >= COLLISIONLIMIT) {
			cube.row = cubeShadow.row;
			cube.col = cubeShadow.col;
			rowVel = 0;
			colVel = 0;
			break;
		} else {
			t = t + dt;
			cube.row = cube.row - dt*rowVel;
			cube.col = cube.col - dt*colVel;
			if (DEBUG) {
				char b[4];
				drawRect(0, 100, 18, 8, BLACK);
				sprintf(b, "%d", cube.row);
				drawString(0, 100, b, RED);
				drawRect(0, 120, 18, 8, BLACK);
				sprintf(b, "%d", cube.col);
				drawString(0, 120, b, RED);
			}
			n++;
		}
	}
	if (DEBUG) {
		drawRect(0, 0, 5, 5, BLUE);
	}
}

int isColliding(Image *i) {
	//determines whether the given image is colliding with any tiles,
	//and updates the in and out portal pointers if it is in any portals

	int inAnyPortals = FALSE;
	int result = FALSE;

	//((blue -> tile == t || blue -> other == t) && (blue -> topLeft != blue -> vertical))
	//bottom-left
	Tile *t = getTileAt(i -> row + i -> height - 1, i -> col);
	if (blue -> tile == t || blue -> other == t) {
		inPortal = blue;
		outPortal = orange;
		inAnyPortals = TRUE;
	} else if (orange -> tile == t || orange -> other == t) {
		inPortal = orange;
		outPortal = blue;
		inAnyPortals = TRUE;
	} else if (t -> type != EMPTY) {
		result = TRUE;
	}

	//((blue -> tile == t || blue -> other == t) &&	blue -> topLeft)
	//bottom-right
	t = getTileAt(i -> row + i -> height - 1, i -> col + i -> width - 1);
	if (blue -> tile == t || blue -> other == t) {
		inPortal = blue;
		outPortal = orange;
		inAnyPortals = TRUE;
	} else if (orange -> tile == t || orange -> other == t) {
		inPortal = orange;
		outPortal = blue;
		inAnyPortals = TRUE;
	} else if (t -> type != EMPTY) {
		result = TRUE;
	}

	//((blue -> tile == t || blue -> other == t) && !blue -> topLeft)
	//top-left
	t = getTileAt(i -> row, i -> col);
	if (blue -> tile == t || blue -> other == t) {
		inPortal = blue;
		outPortal = orange;
		inAnyPortals = TRUE;
	} else if (orange -> tile == t || orange -> other == t) {
		inPortal = orange;
		outPortal = blue;
		inAnyPortals = TRUE;
	} else if (t -> type != EMPTY) {
		result = TRUE;
	}


	//((blue -> tile == t || blue -> other == t) && (blue -> topLeft == blue -> vertical))
	//top-right
	t = getTileAt(i -> row, i -> col + i -> width - 1);
	if (blue -> tile == t || blue -> other == t) {
		inPortal = blue;
		outPortal = orange;
		inAnyPortals = TRUE;
	} else if (orange -> tile == t || orange -> other == t) {
		inPortal = orange;
		outPortal = blue;
		inAnyPortals = TRUE;
	} else if (t -> type != EMPTY) {
		result = TRUE;
	}

	if (!inAnyPortals) {
		inPortal = NULL;
		outPortal = NULL;
	}

	return result;
}

int hasGround(Image *i) {
	//determines if the given image has a tile directly under it

	//prevents hitting tile below when in a portal
	if (inPortal && inPortal -> vertical == FALSE) {
		return FALSE;
	}

	//bottom-left
	Tile *t = getTileAt(i -> row + i -> height, i -> col);
	if (blue -> tile == t || blue -> other == t) {

	} else if (orange -> tile == t || orange -> other == t) {

	} else if (t -> type != EMPTY) {
		return TRUE;
	}

	//bottom-right
	t = getTileAt(i -> row + i -> height, i -> col + i -> width - 1);
	if (blue -> tile == t || blue -> other == t) {

	} else if (orange -> tile == t || orange -> other == t) {

	} else if (t -> type != EMPTY) {
		return TRUE;
	}

	return FALSE;
}

int hasLeftWall(Image *i) {
	//determines if the given image has a tile directly to the left

	//bottom-left
	Tile *t = getTileAt(i -> row + i -> height - 1, i -> col - 1);
	if ((blue -> tile == t || blue -> other == t) ||
			(orange -> tile == t || orange -> other == t)) {

	} else if (t -> type != EMPTY) {
		return TRUE;
	}

	//top-left
	t = getTileAt(i -> row, i -> col - 1);
	if ((blue -> tile == t || blue -> other == t) ||
			(orange -> tile == t || orange -> other == t)) {

	} else if (t -> type != EMPTY) {
		return TRUE;
	}

	return FALSE;
}

int hasRightWall(Image *i) {
	//determines if the given image has a tile directly to the right

	//bottom-right
	Tile *t = getTileAt(i -> row + i -> height - 1, i -> col + i -> width);
	if ((blue -> tile == t || blue -> other == t) ||
			(orange -> tile == t || orange -> other == t)) {

	} else if (t -> type != EMPTY) {
		return TRUE;
	}

	//top-right
	t = getTileAt(i -> row, i -> col + i -> width);
	if ((blue -> tile == t || blue -> other == t) ||
			(orange -> tile == t || orange -> other == t)) {

	} else if (t -> type != EMPTY) {
		return TRUE;
	}

	return FALSE;
}

void callButtons() {
	//updates all key variables and if appropriate
	//calls their associated functions
	rightDown = KEY_DOWN_NOW(BUTTON_RIGHT);
	leftDown = KEY_DOWN_NOW(BUTTON_LEFT);
	upDown = KEY_DOWN_NOW(BUTTON_UP);
	aDown = KEY_DOWN_NOW(BUTTON_A);
	selectDown = KEY_DOWN_NOW(BUTTON_SELECT);
	downDown = KEY_DOWN_NOW(BUTTON_DOWN);

	if (!aWasReleased && !aDown) {
		aWasReleased = 1;
	}
	if (aDown && aWasReleased) {
		aWasReleased = 0;
		onAPressed();
	}
	if (rightDown) {
		onRightHeld();
	}

	if (leftDown) {
		onLeftHeld();
	}

	if (upDown) {
		onUpHeld();
	}

	if (!selectWasReleased && !selectDown) {
		selectWasReleased = 1;
	}
	if (selectDown && selectWasReleased) {
		selectWasReleased = 0;
		onSelectPressed();
	}
}

Tile* createLevel(const int level[]) {
	//converts an array of ints into an array of tiles
 	//makes it easier to create levels by hand
	Tile *levelTiles = malloc(sizeof(Tile) * 150);
	for (int r = 0; r < 10; r++) {
		for (int c = 0; c < 15; c++) {
			TileType type;
			switch (level[OFFSET(r, c, 15)]) {
				case (0):
					type = EMPTY;
					break;
				case (1):
					type = NO_PORTALS;
					break;
				case (2):
					type = PORTALS;
					break;
				case (3):
					type = DOOR;
					break;
				case (4):
					type = POISON;
					break;
				case (5):
					type = PORTAL_GUN;
					break;
				default:
					type = EMPTY;
					break;
			}
			levelTiles[OFFSET(r, c, 15)] = (Tile){r*TILE_HEIGHT, c*TILE_WIDTH, type};
			if (type == DOOR) {
				exitTile = &levelTiles[OFFSET(r, c, 15)];
			}
		}
	}
	return levelTiles;
}

Portal newPortal(Tile *tile, int vertical, int topLeft, PortalType type) {
	//creates a new portal with the given parameters
	return (Portal){tile, getOtherTile(tile, vertical), vertical, topLeft, type};
}

Tile* getOtherTile(Tile *tile, int vertical) {
	//determines the 'other' tile of a portal, which will either be below or
	//to the right of the 'main' tile depending on the portal's parameters
	return vertical ? getTileAt(tile -> row + TILE_HEIGHT, tile -> col) :
		getTileAt(tile -> row, tile -> col + TILE_WIDTH);
}

Tile* getTileAt(int row, int col) {
	//returns the tile at the given row and column in the current tile board
	if (row < 0 || row >= HEIGHT || col < 0 || col >= WIDTH) {
		return NULL;
	}
	return (&tiles[OFFSET(row / TILE_HEIGHT, col / TILE_WIDTH, 15)]);
}

int insidePortal(Image *i, Portal *p) {
	//determines whether the given image is conpletely inside of the given portal
	if (p -> vertical) {
		if (p -> topLeft) {
			return (i -> col >= p -> tile -> col + 1);
		} else {
			return (i -> col + i -> width - 1 <= p -> tile -> col + TILE_WIDTH);
		}
	} else {
		if (p -> topLeft) {
			return (i -> row >= p -> tile -> row - 1);
		} else {
			return (i -> row + i -> height - 1 <= p -> tile -> row + TILE_HEIGHT);
		}
	}
}

void firePortal() {
	//fires and if appropriate creates a portal in the direction
	//corresponding to the user's keypresses
	if (blue) {
		waitForVblank();
		drawTile(blue -> tile);
		drawTile(blue -> other);
	}
	int colVel = leftDown ? -1 : (rightDown ? 1 : 0);
	int rowVel = upDown ? -1 : (downDown ? 1 : 0);
	if (colVel == 0 && rowVel == 0) {
		return;
	}
	int row = cube.row + cube.height/2;
	int col = cube.col + cube.width/2;
	Tile *t = getTileAt(row, col);
	Tile *other;
	//search in a straight line through empty tiles until a tile is hit
	while (t -> type == EMPTY && row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH) {
		row = row + rowVel;
		col = col + colVel;
		t = getTileAt(row, col);
	}

	//if the hit tile is a portal tile and not already a portal, try and add a portal
	if ((t -> type == PORTALS) && (orange -> tile != t) && (orange -> other != t)) {
		int foundOther = FALSE;
		for (int dr = -1; dr <= 1 && !foundOther; dr++) {
			for (int dc = -1; dc <= 1 && !foundOther; dc++) {
				if ((dr == 0 || dc == 0) && (dr != dc)) {
					other = getTileAt(row + dr*TILE_HEIGHT, col + dc*TILE_WIDTH);
					if ((other -> type == PORTALS) && (orange -> tile != other) && (orange -> other != other)) {
						//check that opening is empty
						int rowDif = other -> row - t -> row;
						int colDif = other -> col - t -> col;
						if (colDif < 0) {
							if (rowVel > 0) {
								if (getTileAt(other -> row - 1, other -> col) -> type == EMPTY &&
										getTileAt(other -> row - 1, other -> col + TILE_WIDTH) -> type == EMPTY) {
										foundOther= TRUE;
									}
							} else {
								if (getTileAt(other -> row + TILE_HEIGHT, other -> col) -> type == EMPTY &&
										getTileAt(other -> row + TILE_HEIGHT, other -> col + TILE_WIDTH) -> type == EMPTY) {
										foundOther= TRUE;
								}
							}
						} else if (rowDif < 0) {
							if (colVel > 0) {
								if (getTileAt(other -> row, other -> col - 1) -> type == EMPTY &&
										getTileAt(other -> row + TILE_HEIGHT, other -> col - 1) -> type == EMPTY) {
										foundOther= TRUE;
								}
							} else {
								if (getTileAt(other -> row, other -> col + TILE_WIDTH) -> type == EMPTY &&
										getTileAt(other -> row + TILE_HEIGHT, other -> col + TILE_WIDTH) -> type == EMPTY) {
										foundOther= TRUE;
								}
							}
						} else if (colDif > 0) {
							if (rowVel > 0) {
								if (getTileAt(other -> row - 1, other -> col) -> type == EMPTY &&
										getTileAt(other -> row - 1, other -> col - TILE_WIDTH) -> type == EMPTY) {
										foundOther= TRUE;
									}
							} else {
								if (getTileAt(other -> row + TILE_HEIGHT, other -> col) -> type == EMPTY &&
										getTileAt(other -> row + TILE_HEIGHT, other -> col - TILE_WIDTH) -> type == EMPTY) {
										foundOther= TRUE;
								}
							}
						} else if (rowDif > 0) {
							if (colVel > 0) {
								if (getTileAt(other -> row, other -> col - 1) -> type == EMPTY &&
										getTileAt(other -> row - TILE_HEIGHT, other -> col - 1) -> type == EMPTY) {
										foundOther= TRUE;
								}
							} else {
								if (getTileAt(other -> row, other -> col + TILE_WIDTH) -> type == EMPTY &&
										getTileAt(other -> row - TILE_HEIGHT, other -> col + TILE_WIDTH) -> type == EMPTY) {
										foundOther= TRUE;
								}
							}
						}

					}
				}
			}
		}
		//place portal if a second square to place it on was found
		if (foundOther) {
			int rowDif = other -> row - t -> row;
			int colDif = other -> col - t -> col;

			if (colDif < 0) {
				p1 = newPortal(other, FALSE, rowVel > 0, PORTAL_BLUE);
			} else if (rowDif < 0) {
				p1 = newPortal(other, TRUE, colVel > 0, PORTAL_BLUE);
			} else if (colDif > 0) {
				p1 = newPortal(t, FALSE, rowVel > 0, PORTAL_BLUE);
			} else if (rowDif > 0) {
				p1 = newPortal(t, TRUE, colVel > 0, PORTAL_BLUE);
			}
			blue = &p1;
			waitForVblank();
			drawPortal(blue);
		}
	}
}

void onRightHeld() {
	//function to be called any time the right button is active
	if (!rightWall) {
		colVel = reverseFramesLeft ? -2 : 2;
	}
}

void onLeftHeld() {
	//function to be called any time the left button is active
	if (!leftWall) {
		colVel = reverseFramesLeft ? 2 : -2;
	}
}

void onUpHeld() {
	//function to be called any time the up button is active
	if (ground) {
		rowVel = -5;
	}
}

void onAPressed() {
	//function to be called whenever the A button is pressed
	//(i.e will not be called again until the button is depressed)
	switch (level) {
		case 1:
			if (state == 1 || state == 3) {
				state++;
			}
			break;
		case 3:
			if (state == 1 || state == 3) {
				state++;
			}
			break;
		case 4:
			if (state == 1 || state == 3) {
				state++;
			} else if (state == 5) {
				firePortal();
			}
			break;
		case 5:
			firePortal();
			break;
		case 6:
			firePortal();
			break;
		case 7:
			firePortal();
			break;
	}
}

void onSelectPressed() {
	//function to be called whenever the select button is pressed
	//(i.e will not be called again until the button is depressed)
	startScreen();
}

int insideTile(Image *i, Tile *t) {
	//determines whether the given image is colliding with the given tile
	return ((i -> row >= t -> row) && (i -> col >= t -> col) &&
					(i -> row + i -> height <= t -> row + TILE_HEIGHT) &&
					(i -> col + i -> width <= t -> col + TILE_WIDTH));
}

int touchingTile(Image *i, Tile *t) {
	//determines whether the given image is touching the given tile
	//(i.e. one pixel away from)

	//left
	Tile *t2 = getTileAt(i -> row + (i -> height) / 2, i -> col - 1);
	if (t == t2) {
		return TRUE;
	}

	//right
	t2 = getTileAt(i -> row + (i -> height) / 2, i -> col + i -> width);
	if (t == t2) {
		return TRUE;
	}

	//top
	t2 = getTileAt(i -> row - 1, i -> col + (i -> width) / 2);
	if (t == t2) {
		return TRUE;
	}

	//bottom
	t2 = getTileAt(i -> row + i -> height, i -> col + (i -> width) / 2);
	if (t == t2) {
		return TRUE;
	}

	return FALSE;
}

void openExit() {
	//animates the opening of the level's exit tile
	if (exitTile) {
		Tile *top = getTileAt(exitTile -> row - 1, exitTile -> col);
		for (int i = 0; i < 32; i++) {
			waitForVblank();
			drawRect(exitTile -> row + TILE_HEIGHT - 1 - i/2, exitTile -> col,
				TILE_WIDTH, i/2 + 1, BLACK);
			drawImage3(exitTile -> row - i / 2, exitTile -> col, TILE_WIDTH, TILE_HEIGHT,
				door);
			drawTile(top);
		}
		exitTile -> type = EMPTY;
		drawTile(exitTile);
	}
}

void closeExit() {
	//animates the closing of the level's exit tile
	if (exitTile) {
		Tile *top = getTileAt(exitTile -> row - 1, exitTile -> col);
		for (int i = 31; i >= 0; i--) {
			waitForVblank();
			drawImage3(exitTile -> row - i / 2, exitTile -> col, TILE_WIDTH, TILE_HEIGHT,
				door);
			drawTile(top);
		}
		exitTile -> type = DOOR;
		drawTile(exitTile);
	}
}

void elevatorAnim(Tile *nextLevel, int cubeFinalRow) {
	//animates the elevator that moves the cube from one level to the next
	p1 = (Portal) {NULL, NULL, FALSE, FALSE, EMPTY};
	p2 = p1;
	inPortal = NULL;
	outPortal = NULL;
	orange = NULL;
	blue = NULL;
	int startRow = (cube.row + cube.height - TILE_HEIGHT)/TILE_HEIGHT;
	int finalRow = cubeFinalRow*TILE_HEIGHT + (TILE_HEIGHT - cube.height);
	Tile rightWall[10];
	Tile leftWall[10];
	Tile platform;
	platform = (Tile){cube.row + cube.height, WIDTH + TILE_WIDTH, NO_PORTALS};
	for (int i = 0; i < 10; i++) {
		leftWall[i] = (Tile){i*TILE_HEIGHT, WIDTH, PORTALS};
		if (i == startRow) {
			leftWall[i] = (Tile){i*TILE_HEIGHT, WIDTH, EMPTY};
		}
		rightWall[i] = (Tile){i*TILE_HEIGHT, WIDTH + 2*TILE_WIDTH, PORTALS};
	}

	//shift old level to the left
	for (int i = 0; i < 14*TILE_WIDTH / 4; i++) {
		for (int i = 0; i < 10; i++) {
			leftWall[i].col = leftWall[i].col - 4;
			rightWall[i].col = rightWall[i].col - 4;
		}
		platform.col = platform.col - 4;

		waitForVblank();
		shiftScreenHoriz(-4, BLACK);
		if (i <= TILE_WIDTH) {
			for (int i = 0; i < 10; i++) {
				drawTile(&rightWall[i]);
				drawTile(&leftWall[i]);
			}
			drawTile(&platform);
		}
	}

	//move cube out of old level
	cube.col = 0;
	cubeShadow.row = cube.row;
	for (int i = 0; i < 2*TILE_WIDTH + 2; i++) {
		cube.col = cube.col + 1;
		cubeShadow.col  = cube.col - 1;
		waitForVblank();
		drawRectangle(&cubeShadow);
		drawImage(&cube);
	}

	//move cube to final position
	Rectangle platformShadow = (Rectangle){platform.row, platform.col,
		TILE_WIDTH, TILE_HEIGHT, BLACK};
	int dif = finalRow - cube.row;
	int sign = dif >= 0 ? 1 : -1;
	cubeShadow.col = cube.col;
	for (int i = 0; i < (dif >= 0 ? dif : -dif); i++) {
		platformShadow.row = platform.row;
		cubeShadow.row = cube.row;
		platform.row = platform.row + sign;
		cube.row = cube.row + sign;
		waitForVblank();
		drawRectangle(&cubeShadow);
		drawRectangle(&platformShadow);
		drawTile(&platform);
		drawImage(&cube);
	}

	//animate ascent
	Tile right = (Tile){-TILE_HEIGHT, 3*TILE_WIDTH, PORTALS};
	Tile left = (Tile){-TILE_HEIGHT, TILE_WIDTH, PORTALS};
	for (int i = 0; i < 200; i++) {
		right.row = right.row + 2;
		if (right.row >= 1) {
			right.row = right.row - TILE_HEIGHT;
		}
		left.row = right.row;
		waitForVblank();
		shiftScreenVert(2, BLACK);
		drawRect(platform.row + TILE_HEIGHT, 2*TILE_WIDTH, TILE_WIDTH, 4, BLACK);
		drawTile(&platform);
		drawImage(&cube);
		drawTile(&right);
		drawTile(&left);
	}


	TileType old = nextLevel[OFFSET(cubeFinalRow, 0, 15)].type;
	nextLevel[OFFSET(cubeFinalRow, 0, 15)].type = EMPTY;
	for (int i = 0; i < 150; i++) {
		nextLevel[i].row = -TILE_HEIGHT;
		nextLevel[i].col = nextLevel[i].col + 4*TILE_WIDTH;
	}

	//animate new level coming into view from above
	for (int i = 0; i < HEIGHT; i++) {
		for (int c = 0; c < 11; c++) {
			nextLevel[OFFSET(9 - i / 16, c, 15)].row++;
		}
		right.row = right.row + 1;
		if (right.row >= 1) {
			right.row = right.row - TILE_HEIGHT;
		}
		left.row = right.row;
		waitForVblank();
		shiftScreenVert(1, BLACK);
		drawRect(platform.row + TILE_HEIGHT, 2*TILE_WIDTH, TILE_WIDTH, 4, BLACK);
		drawTile(&platform);
		drawImage(&cube);
		drawTile(&right);
		drawTile(&left);
		for (int c = 0; c < 11; c++) {
			drawTile(&nextLevel[OFFSET(9 - i / 16, c, 15)]);
		}
	}
	nextLevel[OFFSET(cubeFinalRow, 0, 15)].type = old;

	//open elevator & chamber doors
	right = (Tile){cubeFinalRow*TILE_HEIGHT, 3*TILE_WIDTH, PORTALS};
	Tile above = (Tile){(cubeFinalRow - 1)*TILE_HEIGHT, 3*TILE_WIDTH, PORTALS};
	for (int r = 0; r < 16; r++) {
		right.row--;
		waitForVblank();
		drawTile(&right);
		drawTile(&above);
		drawRect(right.row + TILE_HEIGHT, 3*TILE_WIDTH, TILE_WIDTH, 1, BLACK);
	}

	//move cube into chamber
	cubeShadow.row = cube.row;
	for (int c = 0; c < 3*TILE_WIDTH; c++) {
		cubeShadow.col = cube.col;
		cube.col++;
		waitForVblank();
		drawRectangle(&cubeShadow);
		drawImage(&cube);
	}

	//close elevator & chamber doors
	Tile otherAbove = (Tile){(cubeFinalRow - 1)*TILE_HEIGHT, 4*TILE_WIDTH,
		tiles[OFFSET(cubeFinalRow - 1, 0, 15)].type};
	Tile fakeDoor = (Tile){(cubeFinalRow - 1)*TILE_HEIGHT, 4*TILE_WIDTH, old};
	for (int r = 0; r < 16; r++) {
		right.row++;
		fakeDoor.row++;
		waitForVblank();
		drawTile(&right);
		drawTile(&fakeDoor);
		drawTile(&above);
		drawTile(&otherAbove);
	}

	for (int c = 11; c < 15; c++) {
		for (int r = 0; r < 10; r++) {
			nextLevel[OFFSET(r, c, 15)].row = r * TILE_HEIGHT;
			nextLevel[OFFSET(r, c, 15)].col = WIDTH;
		}
	}

	//animate shift new level in from right
	for (int i = 0; i < 4*TILE_WIDTH; i++) {
		for (int r = 0; r < 10; r++) {
			nextLevel[OFFSET(r, 11 + i / TILE_WIDTH, 15)].col--;
		}
		waitForVblank();
		shiftScreenHoriz(-1, BLACK);
		for (int r = 0; r < 10; r++) {
			drawTile(&nextLevel[OFFSET(r, 11 + i / TILE_WIDTH, 15)]);
		}
	}

	//make sure new level tiles are at the proper positions
	for (int c = 0; c < 15; c++) {
		for (int r = 0; r < 10; r++) {
			nextLevel[OFFSET(r, c, 15)].row = r * TILE_HEIGHT;
			nextLevel[OFFSET(r, c, 15)].col = c * TILE_WIDTH;
		}
	}
	cube.col = TILE_WIDTH + 2;
	cube.row = finalRow;
	blue = NULL;
	orange = NULL;
}

void exitAnim() {
	//animates the elevator that moves the cube to the exit
	tiles = NULL;
	p1 = (Portal) {NULL, NULL, FALSE, FALSE, EMPTY};
	p2 = p1;
	inPortal = NULL;
	outPortal = NULL;
	orange = NULL;
	blue = NULL;
	int startRow = (cube.row + cube.height - TILE_HEIGHT)/TILE_HEIGHT;
	Tile rightWall[10];
	Tile leftWall[10];
	Tile platform;
	platform = (Tile){cube.row + cube.height, WIDTH + TILE_WIDTH, NO_PORTALS};
	for (int i = 0; i < 10; i++) {
		leftWall[i] = (Tile){i*TILE_HEIGHT, WIDTH, PORTALS};
		if (i == startRow) {
			leftWall[i] = (Tile){i*TILE_HEIGHT, WIDTH, EMPTY};
		}
		rightWall[i] = (Tile){i*TILE_HEIGHT, WIDTH + 2*TILE_WIDTH, PORTALS};
	}

	//shift old level to the left
	for (int i = 0; i < 14*TILE_WIDTH / 4; i++) {
		for (int i = 0; i < 10; i++) {
			leftWall[i].col = leftWall[i].col - 4;
			rightWall[i].col = rightWall[i].col - 4;
		}
		platform.col = platform.col - 4;

		waitForVblank();
		shiftScreenHoriz(-4, BLACK);
		if (i <= TILE_WIDTH) {
			for (int i = 0; i < 10; i++) {
				drawTile(&rightWall[i]);
				drawTile(&leftWall[i]);
			}
			drawTile(&platform);
		}
	}

	//move cube out of old level
	cube.col = 0;
	cubeShadow.row = cube.row;
	for (int i = 0; i < 2*TILE_WIDTH + 2; i++) {
		cube.col = cube.col + 1;
		cubeShadow.col  = cube.col - 1;
		waitForVblank();
		drawRectangle(&cubeShadow);
		drawImage(&cube);
	}

	//move cube to final position
	Rectangle platformShadow = (Rectangle){platform.row, platform.col,
		TILE_WIDTH, TILE_HEIGHT, BLACK};
	int dif = -cube.height - TILE_HEIGHT - cube.row;
	int sign = dif >= 0 ? 1 : -1;
	cubeShadow.col = cube.col;
	for (int i = 0; i < (dif >= 0 ? dif : -dif); i++) {
		platformShadow.row = platform.row;
		cubeShadow.row = cube.row;
		platform.row = platform.row + sign;
		cube.row = cube.row + sign;
		waitForVblank();
		drawRectangle(&cubeShadow);
		drawRectangle(&platformShadow);
		drawTile(&platform);
		drawImage(&cube);
	}
	for (int r = 0; r < 160; r++) {
		waitForVblank();
		for (int c = 0; c < 240; c++) {
			setPixel(r, c, BLACK);
		}
	}

	drawImage3(0, 0, WIDTH, HEIGHT, endScreen);

	cube.row = -TILE_HEIGHT;
	cube.col = TILE_WIDTH;
	rowVel = 0;
	colVel = 0;
	while(1) {
		callButtons();
		if (cube.row <= 8*TILE_HEIGHT) {
			cubeShadow.row = cube.row;
			cubeShadow.col = cube.col;
			cube.row = cube.row + rowVel;
			rowVel = rowVel + g/2 - rowVel*airFriction;
			waitForVblank();
			drawRectangle(&cubeShadow);
			drawImage(&cube);
		}
	}
}
