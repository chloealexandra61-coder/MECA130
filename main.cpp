
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       carterchlo                                                */
/*    Created:      16/07/2026, 14:08:29                                      */
/*    Description:  IQ2 project                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"
vex::brain       Brain;
#define GROUP_NUMBER 35
#define WALL_THRESHOLD 100
using namespace vex;
#define WAIT_TIME 100
#define ARY_SZ 10 // array size
#define S_RST_EGGS 1 //x
#define S_RST_WHY 1 //y
#define BUFFER_SIZE 10 
#define WALL_CLEAR_MM 170
#define WALL_SET_MM 150
#define UPPER_RED 20
#define UPPER_PINK 30       //these were find from experimenting
#define UPPER_ORANGE 40
#define UPPER_WHITE 45
#define UPPER_YELLOW 55
#define UPPER_GREEN 65
#define UPPER_BLUE 80
int count = 0;
int ind = 0;
double rding; // the reading
double hueShortForHue; // hue is short for hue
double brtns; //brightness
distance dSens = distance(PORT5);
optical opSens = optical(PORT1);
double dstRding[BUFFER_SIZE]; // distance reading array of specific size
touchled tLed = touchled(PORT10);
bool wall = 0; // if wall then 1, if no wall 0.
double oHueShortForHueAverage = 0;
double oBrtnsAverage = 0;
double hueShortForHueRding[BUFFER_SIZE];
double brtnsRding[BUFFER_SIZE];



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
}Tile;

void bufferWrite(double value){
    dstRding[ind] = value;
    ind = (ind + 1) % BUFFER_SIZE;

    if(count < BUFFER_SIZE){
        count++;
    }

}



double average(){
double dTotal = 0;
    for(int i = 0; i < count; i++){
        dTotal += dstRding[i];
    }
    double average = dTotal / count;
    return average;

}

void wallDetect(double distance){
    if(wall){
        if(distance > WALL_CLEAR_MM){
            wall = 0;
        }
    }

    if(!wall){
        if(distance < WALL_SET_MM){
            wall = 1;
        }
    }
}

Tile TileType(){
    Tile color = tile_black;
    double oBrtnsTotal = 0;
    oBrtnsAverage = 0;
    double oHueShortForHueTotal = 0;
    oHueShortForHueAverage = 0;
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

int main(){
    opSens.setLight(ledState::on);
    
    while(1){

        double dAverage = 0;
        double dCurrent = 0;
        

        dCurrent = dSens.objectDistance(mm);
        bufferWrite(dCurrent);

        
        dAverage = average();
        wallDetect(dAverage);
        Tile currentTile = TileType();
        Brain.Screen.print("Avg: %.1f", dAverage);
        Brain.Screen.newLine();
        Brain.Screen.print("Current: %.1f", dCurrent);
        Brain.Screen.newLine();
        if(wall){
            Brain.Screen.print("Walll!");
        } else {
            Brain.Screen.print("No wall :p");
        }
        Brain.Screen.newLine();
        Brain.Screen.print("H: %.1f", oHueShortForHueAverage);
        Brain.Screen.print(" -B: %.1f", oBrtnsAverage);
        Brain.Screen.newLine();

        
        switch (currentTile){
            case 0:
            Brain.Screen.print("normal");
            break;
            case 1:
            Brain.Screen.print("hazard");
            break;
            case 2:
            Brain.Screen.print("pink?");
            break;
            case 3:
            Brain.Screen.print("orange?");
            break;
            case 4:
            Brain.Screen.print("starts");
            break;
            case 5:
            Brain.Screen.print("waypoint");
            break;
            case 6:
            Brain.Screen.print("finish");
            break;
            case 7:
            Brain.Screen.print("Checkpoint");
            break;

        }

        


        
        wait(WAIT_TIME, msec);
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    }
















}
