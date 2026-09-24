#include "sensorsOfHubert.h"
#include "vex.h"
vex::brain       Brain;
vex::distance dSens = vex::distance(vex::PORT5);
vex::optical opSens = vex::optical(vex::PORT1);

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
double cDistance = 800;
tile currentTile;
bool renderMap = false;
int testColour = 0;


tile tileCheck(){
    testColour = 0;
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
    if(oBrtnsAverage < 0.1){
        color = tile_black;
        testColour = 0;
    } else {
        if(oHueShortForHueAverage < UPPER_RED){
            color = tile_red;
            testColour = 1;
        } else {
            if(oHueShortForHueAverage < UPPER_ORANGE){
                color = tile_orange;
                testColour = 2;
            } else {
                if(oHueShortForHueAverage < UPPER_WHITE){
                    color = tile_white;
                    testColour = 3;
                } else {
                    if(oHueShortForHueAverage < UPPER_GREEN){
                        color = tile_green;
                        testColour = 4;
                    } else {
                        if(oHueShortForHueAverage < UPPER_BLUE){
                            color = tile_blue;
                            testColour = 5;
                        } else { color = tile_red;}
                    }
                }
             }
         }
}
    return(color);
}

void colourCheck(){
    //if(!renderMap){
    Brain.Screen.print("clr Check");
    //    }
    currentTile = tileCheck();
 }

    
void wallCheckLong(){
    dTotal = 0;
   for(int i = 0; i < BUFFER_SIZE; i++){
            dstRding[i]  =  dSens.objectDistance(vex::mm);
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
            dstRding[i]  =  dSens.objectDistance(vex::mm);
            dTotal += dstRding[i];
        }
        cDistance = dTotal / BUFFER_SIZE;

        if(cDistance > WALL_CLEAR_MM){
            isWall = 0;
        } else {
            isWall = 1;
        } 
    
}
