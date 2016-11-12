#ifndef MYLIB_H
#define MYLIB_H

#define REG_DISPCNT *(unsigned short*) 0x4000000
#define MODE_3 3
#define BG2_EN (1 << 10)

#define OFFSET(row, col, rowlen)  ((row)*(rowlen)+(col))
#define SCANLINECOUNTER *(volatile unsigned short *)0x4000006
#define WIDTH 240
#define HEIGHT 160

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define PORTAL_THICKNESS 2

#define COLOR(R,G,B) (((R) & 0x1F) | (((G) & 0x1F) << 5) | (((B) & 0x1F) << 10))
#define BLACK   COLOR(0 , 0 , 0 )
#define WHITE   COLOR(31, 31, 31)
#define RED     COLOR(31, 0 , 0 )
#define GREEN   COLOR(0 , 31, 0 )
#define BLUE    COLOR(0 , 0 , 31)
#define YELLOW  COLOR(31, 31, 0 )
#define MAGENTA COLOR(31, 0 , 31)
#define CYAN    COLOR(0 , 31, 31)
#define ORANGE  COLOR(31, 15, 0 )
#define BROWN   COLOR(18, 9 , 0 )
#define PURPLE  COLOR(15, 0 , 15)
#define TEAL    COLOR(0 , 15, 15)
#define MAROON  COLOR(15, 0 , 0 )
#define GREY    COLOR(15, 15, 15)
#define PINK    COLOR(31, 18, 19)

#define BUTTONS       *(volatile unsigned short*) 0x4000130
#define BUTTON_A      (1 << 0)
#define BUTTON_B      (1 << 1)
#define BUTTON_START  (1 << 2)
#define BUTTON_SELECT (1 << 3)
#define BUTTON_RIGHT  (1 << 4)
#define BUTTON_LEFT   (1 << 5)
#define BUTTON_UP     (1 << 6)
#define BUTTON_DOWN   (1 << 7)
#define BUTTON_R      (1 << 8)
#define BUTTON_L      (1 << 9)
#define KEY_DOWN_NOW(key) (~(BUTTONS) & key)

#define REG_DMA3SAD *(volatile unsigned int*)0x40000D4	// source address
#define REG_DMA3DAD *(volatile unsigned int*)0x40000D8	// destination address
#define REG_DMA3CNT *(volatile unsigned int*)0x40000DC	// control register

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)

#define FALSE 0
#define TRUE 1

#define NULL ((void *)0)

struct Vector {
	double row;
	double col;
};
typedef struct Vector Vector;

struct Image {
	int row;
	int col;
	int width;
	int height;
	const unsigned short *image;
};
typedef struct Image Image;

struct Rectangle {
	int row;
	int col;
	int width;
	int height;
	unsigned short color;
};
typedef struct Rectangle Rectangle;

enum TileType {
	EMPTY,
	NO_PORTALS,
	PORTALS,
	DOOR,
	POISON,
	PORTAL_GUN
};
typedef enum TileType TileType;

struct Tile {
	int row;
	int col;
	TileType type;
};
typedef struct Tile Tile;

enum PortalType {
	PORTAL_ORANGE,
	PORTAL_BLUE
};
typedef enum PortalType PortalType;

struct Portal {
	Tile *tile;
	Tile *other;
	int vertical;
	int topLeft;
	PortalType type;
};
typedef struct Portal Portal;

struct Door {
	Tile *tile;
	int open;
};
typedef struct Door Door;


unsigned short *videoBuffer;
void setPixel(int row, int col, unsigned short color);
unsigned short getPixel(int row, int col);
void outlineRect(int row, int col, int width, int height, const unsigned short color);
void drawRect(int row, int col, int width, int height, const unsigned short color);
void outlineRectangle(Rectangle *r);
void drawRectangle(Rectangle *r);
void fillScreen3(unsigned short color);
void shiftScreenHoriz(int colShift, const unsigned short fillColor);
void shiftScreenVert(int rowShift, const unsigned short fillColor);
void waitForVblank(void);
void drawChar(int row, int col, char ch, const unsigned short color);
void drawString(int row, int col, char *str, const unsigned short color);
void drawImage3(int row, int col, int width, int height, const unsigned short *image);
void drawImage(Image *i);
void drawTile(Tile *t);
void drawPortal(Portal *p);

#endif
