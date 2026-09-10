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
#define MAZE_WIDTH 10
#define MAZE_HEIGHT 10


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
bool isWall = 0; // if wall then 1, if no wall 0.
double oHueShortForHueAverage = 0;
double oBrtnsAverage = 0;
double hueShortForHueRding[BUFFER_SIZE];
double brtnsRding[BUFFER_SIZE];
double dAverage = 0;
double align[4];
double dTotal = 0;
double heading = 0;
double cDistance = 0;
int currentX = 0;
int currentY = 0;

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
    wall north;
    wall south;
    wall east;
    wall west;
    bool visited;
    tile tileType;
}cell;


tile currentTile = tile_black;
state gState = STATE_STARTUP;
cell maze[MAZE_WIDTH][MAZE_HEIGHT];


void mazeSetup(){
    maze[0][0].visited = true;
    maze[0][0].west = present;
    maze[0][0].south = present;
    maze[0][0].tileType = tile_white;
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
        dTotal = 0;
        drive.turnToHeading((270), degrees);

        for(int i = 0; i < BUFFER_SIZE; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            dTotal += dstRding[i];
        }
        align[0] = dTotal / BUFFER_SIZE;


        drive.turnToHeading((180), degrees);
        dTotal = 0;
        for(int i = 0; i < BUFFER_SIZE; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            dTotal += dstRding[i];
        }
        align[1] = dTotal / BUFFER_SIZE;
    

    
        if((align[0] - align[1]) > 10){
            alignment = false;
        }
        drive.turnToHeading(0, degrees);
    

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    return alignment;
        
    
    
}



void startup(){
    tLed.on(orange);
    Brain.Screen.print("startup");
    mazeSetup();
    calibrate();
    bool alignment = alignCheck();
    if(alignment){
        gState = STATE_IDLE;
    } else {
        gState = STATE_ERROR;
    }
}

void idle(){
    tLed.on(violet);
    Brain.Screen.print("idle");
    while(WAIT_TIME){
        if(tLed.pressing()){
            gState = STATE_GATHER_INFO;
            break;
        }
    }
}

void colourCheck(){
    Brain.Screen.print("clr Check");
    currentTile = tileCheck();
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

void gatherInfo(){
    tLed.on(purple);
    Brain.Screen.print("gather");
    maze[currentX][currentY].visited = true;
    maze[currentX][currentY].tileType = currentTile;
    if(maze[currentX][currentY].north == unk){
        heading = 0;
        drive.turnToHeading(heading, degrees);
        drive.turnToHeading(heading, degrees);

        wallCheck();
        if(isWall == 0){
            maze[currentX][currentY].north = absent;
            maze[currentX][currentY + 1].south = absent;
        } else {
            maze[currentX][currentY].north = present;
            maze[currentX][currentY + 1].south = present;

        }
    }

    if(maze[currentX][currentY].east == unk){
        heading = 90;
        drive.turnToHeading(heading, degrees);
        drive.turnToHeading(heading, degrees);

        wallCheck();
        if(isWall == 0){
            maze[currentX][currentY].east = absent;
            maze[currentX + 1][currentY].west = absent;

        } else {
            maze[currentX][currentY].east = present;
            maze[currentX + 1][currentY].west = absent;

        }
    }

    if(maze[currentX][currentY].south == unk){
        heading = 180;
        drive.turnToHeading(heading, degrees);
        drive.turnToHeading(heading, degrees);

        wallCheck();
        if(isWall == 0){
            maze[currentX][currentY].south = absent;
            maze[currentX][currentY - 1].north = absent;

        } else {
            maze[currentX][currentY].south = present;
            maze[currentX][currentY - 1].north = present;

        }
    }

    if(maze[currentX][currentY].west == unk){
        heading = 270;
        drive.turnToHeading(heading, degrees);
        drive.turnToHeading(heading, degrees);

        wallCheck();
        if(isWall == 0){
            maze[currentX][currentY].west = absent;
            maze[currentX - 1][currentY].east = absent;

        } else {
            maze[currentX][currentY].west = present;
            maze[currentX - 1][currentY].east = present;

            
        }
    }
    gState = STATE_DECIDE;

}

void decide(){
    tLed.on(blue);
    Brain.Screen.print("think");
    if(maze[currentX][currentY].north == absent && maze[(currentX + 1)][currentY].visited == false){
        heading = 0;
        currentY = currentY + 1;
    } else if(maze[currentX][currentY].east == absent && maze[currentX][(currentY + 1)].visited == false) {
        heading = 90;
        currentX = currentX + 1;
    } else if(maze[currentX][currentY].south == absent && maze[(currentX -1)][currentY].visited == false){
        heading = 180;
        currentY = currentY - 1;
    } else if(maze[currentX][currentY].west == absent && maze[currentX][(currentY - 1)].visited == false){
        heading = 270;
        currentX = currentX - 1;

        // we are stuck!!!

    } else if(maze[currentX][currentY].north == absent && maze[(currentX + 1)][currentY].tileType != tile_red){
        heading = 0;
        currentY = currentY + 1;

    } else if(maze[currentX][currentY].east == absent && maze[currentX][(currentY + 1)].tileType != tile_red) {
        heading = 90;
        currentX = currentX + 1;

    } else if(maze[currentX][currentY].south == absent && maze[(currentX -1)][currentY].tileType != tile_red){
        heading = 180;
        currentY = currentY - 1;
    } else if(maze[currentX][currentY].west == absent && maze[currentX][(currentY - 1)].tileType != tile_red){
        heading = 270;
        currentX = currentX - 1;
    }

    gState = STATE_MOVE;
    
}

void move(){
    tLed.on(green);
    Brain.Screen.print("moving");
    drive.turnToHeading(heading, degrees);
    drive.turnToHeading(heading, degrees);
    drive.driveFor(forward, 400, mm);
    drive.turnToHeading(0, degrees);
    drive.turnToHeading(0, degrees);

    gState = STATE_GATHER_INFO;
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
            case STATE_STARTUP:
            startup();
            break;
            case STATE_IDLE:
            idle();
            break;
            case STATE_GATHER_INFO:
            gatherInfo();
            break;
            case STATE_DECIDE:
            decide();
            break;
            case STATE_MOVE:
            move();
            break;
            case STATE_ERROR:
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
    
