#ifndef SENSORS_OF_HUBERT_H
#define SENSORS_OF_HUBERT_H
#include "vex.h"
extern vex::brain       Brain;
extern vex::distance dSens;
extern vex::optical opSens;

typedef enum{
    tile_black,
    tile_red, 
    tile_pink,
    tile_orange,
    tile_white,
    tile_yellow,
    tile_green,
    tile_blue,
    tile_error,
}tile;

void colourCheck();
void wallCheckLong();
void wallCheck();
tile tileCheck();

#define GROUP_NUMBER 35
#define WALL_THRESHOLD 100
#define BUFFER_SIZE 10 
#define WALL_CLEAR_MM 400
#define WALL_CLEAR_LONG 800
#define UPPER_RED 24
#define UPPER_ORANGE 47
#define UPPER_WHITE 64
#define UPPER_GREEN 75
#define UPPER_BLUE 100

extern double rding; // the reading
extern double hueShortForHue; // hue is short for hue
extern double brtns; //brightness
extern double dstRding[BUFFER_SIZE]; // distance reading array of specific size
extern bool isWall; // if wall then 1, if no wall 0.
extern bool isWallLong; //checks the tile ahead
extern double oHueShortForHueAverage;
extern double oBrtnsAverage;
extern double hueShortForHueRding[BUFFER_SIZE];
extern double brtnsRding[BUFFER_SIZE];
extern double dAverage;
extern double dTotal;
extern double cDistance;
extern tile currentTile;
extern bool renderMap;
extern int testColour;

#endif
