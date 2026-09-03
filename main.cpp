#include "vex.h"
vex::brain       Brain;
#define GROUP_NUMBER 35
#define WALL_THRESHOLD 100
using namespace vex;
#define WAIT_TIME 10
#define ARY_SZ 10 // array size
#define S_RST_EGGS 1 //x
#define S_RST_WHY 1 //y
#define BUFFER_SIZE 10 
#define WALL_CLEAR_MM 400
#define WALL_SET_MM 400
#define UPPER_RED 20
#define UPPER_PINK 30       //these were find from experimenting
#define UPPER_ORANGE 40
#define UPPER_WHITE 45
#define UPPER_YELLOW 55
#define UPPER_GREEN 65
#define UPPER_BLUE 80
#define WHEEL_DIAM 63 //millimetres
#define WHEEL_CIRC (3.14159265 * WHEEL_DIAM)
#define TRACK_WIDTH 143
#define WHEEL_BASE  112.5
#define GEAR_RATIO 1.0


int count = 0;

distance dSens = distance(PORT5);
optical opSens = optical(PORT1);
touchled tLed = touchled(PORT10);
motor lMotor = motor(PORT6, false);
motor rMotor = motor(PORT12, true);
inertial iner = inertial();
smartdrive drive = smartdrive(lMotor, rMotor, iner, WHEEL_CIRC, TRACK_WIDTH, WHEEL_BASE, mm, GEAR_RATIO);

double rding; // the reading
double hueShortForHue; // hue is short for hue
double brtns; //brightness
double dstRding[BUFFER_SIZE]; // distance reading array of specific size
bool wall = 0; // if wall then 1, if no wall 0.
double oHueShortForHueAverage = 0;
double oBrtnsAverage = 0;
double hueShortForHueRding[BUFFER_SIZE];
double brtnsRding[BUFFER_SIZE];
double dAverage = 0;
double align[4];
double dTotal = 0;
double heading = 0;
double cDistance = 0;

typedef enum{
    STATE_STARTUP,
    STATE_IDLE,
    STATE_COLOUR_CHECK,
    STATE_WALL_CHECK,
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

tile currentTile = tile_black;
state gState = STATE_STARTUP;




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


void calibrate(){

    iner.calibrate();
    while(iner.isCalibrating()){
        wait(50, msec);
    }
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
}


bool alignCheck(){
    Brain.Screen.print("aligning lol");
    bool alignment = true;
    for(int i = 0; i < 4; i++){
        dTotal = 0;
        for(int i = 0; i < BUFFER_SIZE; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            dTotal += dstRding[i];
        }
        align[i] = dTotal / BUFFER_SIZE;
        drive.turnToHeading((90 * (i+1) % 360), degrees);
    }

    for(int i = 1; i < 4; i++){
        if((align[0] - align[i]) > 10){
            alignment = false;
        }
    }

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    return alignment;
        
    
    
}



void startup(){
    tLed.on(orange);
    Brain.Screen.print("startup");
    calibrate();
    bool alignment = alignCheck();
    if(alignment){
        gState = STATE_IDLE;
    } else {
        gState = STATE_ERROR;
    }
}

void idle(){
    tLed.on(purple);
    Brain.Screen.print("idle");
    while(WAIT_TIME){
        if(tLed.pressing()){
            gState = STATE_COLOUR_CHECK;
            break;
        }
    }
}

void colourCheck(){
    tLed.on(violet);
    Brain.Screen.print("clr Check");
    currentTile = tileCheck();
        gState = STATE_WALL_CHECK;        
        }



void wallCheck(){
    dTotal = 0;
   for(int i = 0; i < BUFFER_SIZE; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            dTotal += dstRding[i];
        }
        cDistance = dTotal / BUFFER_SIZE;


        if(cDistance > WALL_CLEAR_MM){
            wall = 0;
        } else {wall = 1;
        
        }
    


        
    gState = STATE_DECIDE;
}

void decide(){
    tLed.on(blue);
    Brain.Screen.print("🤔🤔🤔");
    if(wall == 0){
        heading = 0;
    }
    if(wall == 1){
        heading = 90;
       
    }
    gState = STATE_MOVE;
}

void move(){
    tLed.on(green);
    Brain.Screen.print("🏎️🏎️🏎️💨");
    drive.turnToHeading(heading, degrees);
    drive.driveFor(forward, 400, mm);
    gState = STATE_COLOUR_CHECK;
}
    
void error(){
    tLed.on(red);
    while(WAIT_TIME){
        if(tLed.pressing()){
            gState = STATE_IDLE;
            break;
        }
    }
}

void stateMachine(){
    switch (gState){
            case 0:
            startup();
            break;
            case 1:
            idle();
            break;
            case 2:
            colourCheck();
            break;
            case 3:
            wallCheck();
            break;
            case 4:
            decide();
            break;
            case 5:
            move();
            break;
            case 6:
            error();
            break;
        }


    }

    int main(){
    while(WAIT_TIME){
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
        stateMachine();
    }
    }
    
