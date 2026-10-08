#include "sensorsOfHubert.h"
#include "vex.h"
vex::brain       Brain;
vex::distance dSens = vex::distance(vex::PORT5);
vex::optical opSens = vex::optical(vex::PORT1);
vex::touchled tLed = vex::touchled(vex::PORT10);


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
double cDistance = 0;
tile currentTile;
double blackBrtns;
double redCol;
double orangeCol;
double whiteCol;
double greenCol;
double blueCol;
double offput = 2;
bool doColourCheck = false;

void colourSet(){
    if(doColourCheck){
            Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    Brain.Screen.print("Place me on Black");
    wait(250, vex::msec);
    while(!tLed.pressing()){
        wait(50, vex::msec);
    }
    tileCheck();
    blackBrtns = oBrtnsAverage;

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    Brain.Screen.print("Place me on Red");
    wait(250, vex::msec);
    while(!tLed.pressing()){
        wait(50, vex::msec);
    }
    tileCheck();
    redCol = oHueShortForHueAverage;

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    Brain.Screen.print("Place me on Orange");
    wait(250, vex::msec);
    while(!tLed.pressing()){
        wait(50, vex::msec);
    }
    tileCheck();
    orangeCol = oHueShortForHueAverage;

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    Brain.Screen.print("Place me on Blue");
    wait(250, vex::msec);
    while(!tLed.pressing()){
        wait(50, vex::msec);
    }
    tileCheck();
    blueCol = oHueShortForHueAverage;

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    Brain.Screen.print("Place me on Green");
    wait(250, vex::msec);
    while(!tLed.pressing()){
        wait(50, vex::msec);
    }
    tileCheck();
    greenCol = oHueShortForHueAverage;

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    Brain.Screen.print("Place me on White");
    wait(250, vex::msec);
    while(!tLed.pressing()){
        wait(50, vex::msec);
    }
    tileCheck();
    whiteCol = oHueShortForHueAverage;
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    }

}

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

    if(doColourCheck){
        if(oBrtnsAverage < blackBrtns+3){
            color = tile_black;
        } else {
            if(redCol-offput < oHueShortForHueAverage && oHueShortForHueAverage < redCol+offput){
                color = tile_red;
            } else {
                if(orangeCol-offput < oHueShortForHueAverage && oHueShortForHueAverage < orangeCol+offput){
                   color = tile_orange;
                } else {
                    if(whiteCol-offput < oHueShortForHueAverage && oHueShortForHueAverage < whiteCol+offput){
                        color = tile_white;
                    } else {
                        if(blueCol-offput < oHueShortForHueAverage && oHueShortForHueAverage < blueCol+offput){
                            color = tile_blue;
                        } else {
                            if(greenCol-offput < oHueShortForHueAverage && oHueShortForHueAverage < greenCol+offput){
                                color = tile_green;
                            } //else { color = tile_red;}
                        }
                    }
                }
            }
        }
    } else {
        if(oBrtnsAverage < 7){
            color = tile_black;
        } else {
            if(oHueShortForHueAverage < UPPER_RED){
                color = tile_red;
            } else {
                if(oHueShortForHueAverage < UPPER_ORANGE){
                    color = tile_orange;
                } else {
                    if(oHueShortForHueAverage < UPPER_WHITE){
                        color = tile_white;
                    } else {
                        if(oHueShortForHueAverage < UPPER_BLUE){
                            color = tile_blue;
                        } else {
                            if(oHueShortForHueAverage < UPPER_GREEN){
                                color = tile_green;
                            } else { color = tile_red;}
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

void printBrightness(){
    Brain.Screen.print(", %.1f", oBrtnsAverage);
}

void printHue(){
    Brain.Screen.print("%.1f", oHueShortForHueAverage);
}
