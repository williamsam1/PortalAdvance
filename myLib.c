#include <stdio.h>

#include "font.h"
#include "myLib.h"
#include "images/whiteTile.h"
#include "images/greyTile.h"
#include "images/door.h"
#include "images/portalGun.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;

void setPixel(int row, int col, const unsigned short color) {
	if (col >= 0 && col < WIDTH && row >= 0 && row < HEIGHT) {
		videoBuffer[OFFSET(row, col, WIDTH)] = color;
	}
}

unsigned short getPixel(int row, int col) {
	if (col >= 0 && col < WIDTH && row >= 0 && row < HEIGHT) {
		return videoBuffer[OFFSET(row, col, WIDTH)];
	} else {
		return (unsigned short)0;
	}
}

void outlineRect(int row, int col, int width, int height, const unsigned short color) {
	for (int r = row; r < row + height; r++) {
		setPixel(r, col, color);
		setPixel(r, col + width - 1, color);
	}
	for (int c = col; c < col + width; c++) {
		setPixel(row, c, color);
		setPixel(row + width - 1, c, color);
	}

}

void drawRect(int row, int col, int width, int height, const unsigned short color) {
	int w = col + width < WIDTH ? width : WIDTH - col;
	for (int r = 0; r < height && r < HEIGHT; r++) {
		REG_DMA3SAD = (unsigned int)&color;
		REG_DMA3DAD = (unsigned int)(&videoBuffer[OFFSET(row + r, col, WIDTH)]);
		REG_DMA3CNT = w | DMA_ON | DMA_SOURCE_FIXED;
	}
}

void drawRectangle(Rectangle *r) {
	drawRect(r -> row, r -> col, r -> height, r -> width, r -> color);
}

void outlineRectangle(Rectangle *r) {
	drawRect(r -> row, r -> col, r -> height, r -> width, r -> color);
}

void fillScreen3(const unsigned short color) {
	REG_DMA3SAD = (unsigned int)&color;
	REG_DMA3DAD = (unsigned int)videoBuffer;
	REG_DMA3CNT = 38400 | DMA_ON | DMA_SOURCE_FIXED;
}

void waitForVblank() {
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

void drawChar(int row, int col, char ch, const unsigned short color) {
	for(int r = 0; r < 8; r++) {
		for(int c = 0; c < 6; c++) {
			if(fontdata_6x8[OFFSET(r,c,6) + ch*48]) {
				setPixel(row+r, col+c, color);
			}
		}
	}
}

void drawString(int row, int col, char *str, const unsigned short color) {
	while(*str) {
		drawChar(row, col, *str++, color);
		col += 6;
	}
}

void drawImage3(int row, int col, int width, int height, const unsigned short *image) {
	int w = col + width < WIDTH ? width : WIDTH - col;
	if (col < 0) {
		w = width + col;
	}
	if (w > 0) {
		for (int r = 0; r < height && row + r < HEIGHT; r++){
			REG_DMA3SAD = (unsigned int)(&image[OFFSET(r, 0, width)]);
			REG_DMA3DAD = (unsigned int)(&videoBuffer[OFFSET(row + r, col, WIDTH)]);
			REG_DMA3CNT = w | DMA_ON | DMA_SOURCE_INCREMENT;
		}
	}
}


void shiftScreenHoriz(int colShift, const unsigned short fillColor) {
	if (colShift < 0) {
		//shift left
		for (int r = 0; r < HEIGHT; r++) {
			REG_DMA3SAD = (unsigned int)(&videoBuffer[OFFSET(r, -colShift, WIDTH)]);
			REG_DMA3DAD = (unsigned int)(&videoBuffer[OFFSET(r, 0, WIDTH)]);
			REG_DMA3CNT = (WIDTH + colShift) | DMA_ON | DMA_SOURCE_INCREMENT;
		}
		drawRect(0, WIDTH + colShift, -colShift, HEIGHT, fillColor);
	} else if (colShift > 0) {
		//shift right
		for (int r = 0; r < HEIGHT; r++) {
			REG_DMA3SAD = (unsigned int)(&videoBuffer[OFFSET(r, 0, WIDTH)]);
			REG_DMA3DAD = (unsigned int)(&videoBuffer[OFFSET(r, colShift, WIDTH)]);
			REG_DMA3CNT = (WIDTH - colShift) | DMA_ON | DMA_SOURCE_INCREMENT;
		}
		drawRect(0, 0, colShift, HEIGHT, fillColor);
	}
}

void shiftScreenVert(int rowShift, const unsigned short fillColor) {
	if (rowShift > 0) {
		//shift down
		for (int r = HEIGHT - 1; r >= rowShift; r--) {
			REG_DMA3SAD = (unsigned int)(&videoBuffer[OFFSET(r - rowShift, 0, WIDTH)]);
			REG_DMA3DAD = (unsigned int)(&videoBuffer[OFFSET(r, 0, WIDTH)]);
			REG_DMA3CNT = WIDTH | DMA_ON | DMA_SOURCE_INCREMENT;
		}
		drawRect(0, 0, WIDTH, rowShift, fillColor);
	} else if (rowShift < 0) {
		//shift up
		for (int r = 0; r <= HEIGHT + rowShift; r++) {
			REG_DMA3SAD = (unsigned int)(&videoBuffer[OFFSET(r, 0, WIDTH)]);
			REG_DMA3DAD = (unsigned int)(&videoBuffer[OFFSET(r - rowShift, 0, WIDTH)]);
			REG_DMA3CNT = WIDTH | DMA_ON | DMA_SOURCE_INCREMENT;
		}
		drawRect(HEIGHT - 1, 0, WIDTH, -rowShift, fillColor);
	}
}

void drawImage(Image *i) {
	drawImage3(i -> row, i -> col, i -> width, i -> height, i -> image);
}

void drawTile(Tile *t) {
	switch (t -> type) {
		case PORTALS:
			drawImage3(t -> row, t -> col, TILE_WIDTH, TILE_HEIGHT, whiteTile);
			break;
		case NO_PORTALS:
			drawImage3(t -> row, t -> col, TILE_WIDTH, TILE_HEIGHT, greyTile);
			break;
		case DOOR:
			drawImage3(t -> row, t -> col, TILE_WIDTH, TILE_HEIGHT, door);
			break;
		case POISON:
			drawRect(t -> row, t -> col, TILE_WIDTH, TILE_HEIGHT, YELLOW);
			break;
		case EMPTY:
			drawRect(t -> row, t -> col, TILE_WIDTH, TILE_HEIGHT, BLACK);
			break;
		case PORTAL_GUN:
			drawImage3(t -> row, t -> col, TILE_WIDTH, TILE_HEIGHT, portalGun);
			break;
	}
}

void drawPortal(Portal *p) {
	unsigned short color = p -> type == PORTAL_ORANGE ? ORANGE : BLUE;
	if (p -> vertical) {
		if (p -> topLeft) {
			drawRect(p -> tile -> row, p -> tile -> col,
				PORTAL_THICKNESS, 2*TILE_HEIGHT, color);
		} else {
			drawRect(p -> tile -> row, p -> tile -> col + TILE_WIDTH - PORTAL_THICKNESS,
				PORTAL_THICKNESS, 2*TILE_HEIGHT, color);
		}
	} else {
		if (p -> topLeft) {
			drawRect(p -> tile -> row, p -> tile -> col,
				2*TILE_HEIGHT, PORTAL_THICKNESS, color);
		} else {
			drawRect(p -> tile -> row + TILE_HEIGHT - PORTAL_THICKNESS, p -> tile -> col,
				2*TILE_HEIGHT, PORTAL_THICKNESS, color);
		}
	}
}
