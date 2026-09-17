#include "sensorsOfHubert.h"
#include "vex.h"
vex::brain       Brain;
#define GROUP_NUMBER 35
#define WALL_THRESHOLD 100
using namespace vex;
#define BUFFER_SIZE 10 
#define WALL_CLEAR_MM 400
#define WALL_CLEAR_LONG 800
#define UPPER_RED 20
#define UPPER_PINK 30       //these were found from experimenting
#define UPPER_ORANGE 40
#define UPPER_WHITE 45
#define UPPER_YELLOW 55
#define UPPER_GREEN 65
#define UPPER_BLUE 80

distance dSens = distance(PORT5);
optical opSens = optical(PORT1);


double rding; // the reading
double hueShortForHue; // hue is short for hue
double brtns; //brightness
double dstRding[BUFFER_SIZE]; // distance reading array of specific size
bool isWall = 0; // if wall then 1, if no wall 0.
bool isWallLong = 0; //checks the tile ahead
double oHueShortForHueAverage = 0;
double oBrtnsAverage = 0;
double hueShortForHueRding[BUFFER_SIZE];
double brtnsRding[BUFFER_SIZE];
double dAverage = 0;
double dTotal = 0;


typedef enum{
    STATE_STARTUP,
    STATE_IDLE,
    STATE_GATHER_INFO,
    STATE_DECIDE,
    STATE_MOVE,
    STATE_ERROR
}state;


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

typedef enum{
    unk,
    present,
    absent,
}wall;

typedef struct{
    wall north = unk;
    wall south = unk;
    wall east = unk;
    wall west= unk;
    int visited = 0 ;
    tile tileType = tile_black;
}cell;


tile tileCheck(){
    double oBrtnsTotal = 0;
    oBrtnsAverage = 0;
    double oHueShortForHueTotal = 0;
    oHueShortForHueAverage = 0;
    tile color = tile_black;
    for(int i = 0; i < BUFFER_SIZE; i++){
            hueShortForHueRding[i] = opSens.hue();
            oHueShortForHueTotal += hueShortForHueRding[i];

            brtnsRding[i] = opSens.brightness();
            oBrtnsTotal += brtnsRding[i];

        }
    oBrtnsAverage = oBrtnsTotal / BUFFER_SIZE;
    oHueShortForHueAverage =  oHueShortForHueTotal / BUFFER_SIZE;
    if(oBrtnsAverage < 20){
        color = tile_black;
    } else {
        if(oHueShortForHueAverage < UPPER_RED){
            color = tile_red;
        } else {
            if(oHueShortForHueAverage < UPPER_PINK){
                color = tile_pink;
            } else {
                if(oHueShortForHueAverage < UPPER_ORANGE){
                    color=tile_orange;
                } else {
                    if(oHueShortForHueAverage < UPPER_WHITE){
                        color = tile_white;
                    } else {
                        if(oHueShortForHueAverage < UPPER_YELLOW){
                            color = tile_yellow;
                        } else {
                            if(oHueShortForHueAverage < UPPER_GREEN){
                                color = tile_green;
                            } else {
                                if(oHueShortForHueAverage < UPPER_BLUE){
                                    color = tile_blue;
                                } else { color = tile_red;}
                            }
                        }
                    }
                }


        }
    }
}
    return(color);
}

void colourCheck(){
    if(!renderMap){
    Brain.Screen.print("clr Check");
        }
    currentTile = tileCheck();
 }

    
void wallCheckLong(){
    dTotal = 0;
   for(int i = 0; i < BUFFER_SIZE; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            dTotal += dstRding[i];
        }
        cDistance = dTotal / BUFFER_SIZE;

        if(cDistance > WALL_CLEAR_LONG){
            isWallLong = 0;
        } else {
            isWallLong = 1;
        }
}



void wallCheck(){
    dTotal = 0;
   for(int i = 0; i < BUFFER_SIZE; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            dTotal += dstRding[i];
        }
        cDistance = dTotal / BUFFER_SIZE;

        if(cDistance > WALL_CLEAR_MM){
            isWall = 0;
        } else {
            isWall = 1;
        } 
    
}