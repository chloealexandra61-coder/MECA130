#include "vex.h"
//#include "sensorsOfHubert.h"
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
#define WALL_CLEAR_LONG 800
#define UPPER_RED 20
#define UPPER_PINK 30       //these were find from experimenting
#define UPPER_ORANGE 40
#define UPPER_WHITE 45
#define UPPER_YELLOW 55
#define UPPER_GREEN 65
#define UPPER_BLUE 80
#define WHEEL_DIAM 63 //millimetres
#define WHEEL_CIRC (WHEEL_DIAM * 3.14159)
#define TRACK_WIDTH 143
#define WHEEL_BASE  112.5
#define GEAR_RATIO 1.0
#define MAZE_WIDTH 10
#define MAZE_HEIGHT 10
#define TURN_SPEED_FAST 95
#define TURN_SPEED_SLOW 15
#define DRIVE_SPEED 95
#define DRIVE_SPEED_SLOW 40
#define GOOD_WALL_DIST 110
bool renderMap = true;
bool mapOutline = false;
bool wallSquare = true;
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
bool isWallLong = 0; //checks the tile ahead
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
int turnCount = 0;


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

typedef enum{
    north,
    east,
    south,
    west,
}dir;


tile currentTile = tile_black;
state gState = STATE_STARTUP;
cell maze[MAZE_WIDTH][MAZE_HEIGHT];


void updateMap(){

    if(!renderMap)
        return;
    int px = currentX * 10;
    int py = currentY * 10;

    // NORTH

    if(maze[currentX][currentY].north == present){
        Brain.Screen.setPenColor(white);
        Brain.Screen.drawLine(px,100 - py,px + 10,100 - py);
    }

    if(maze[currentX][currentY].north == absent){
        Brain.Screen.setPenColor(blue);
        Brain.Screen.drawLine(px,100 - py,px + 10,100 - py);
    }


    // east

    if(maze[currentX][currentY].east == present){
        Brain.Screen.setPenColor(white);
        Brain.Screen.drawLine(px + 10,100 - py,px + 10,100 - py + 10);
    }

    if(maze[currentX][currentY].east == absent){
        Brain.Screen.setPenColor(blue);
        Brain.Screen.drawLine(px + 10,100 - py,px + 10,100 - py + 10);
    }


    
    // south
   

    if(maze[currentX][currentY].south == present){
        Brain.Screen.setPenColor(white);
        Brain.Screen.drawLine(px,100 - py + 10,px + 10,100 - py + 10);
    }

    if(maze[currentX][currentY].south == absent){
        Brain.Screen.setPenColor(blue);
        Brain.Screen.drawLine(px,100 - py + 10,px + 10,100 - py + 10);
    }


    // west
    if(maze[currentX][currentY].west == present){
        Brain.Screen.setPenColor(white);
        Brain.Screen.drawLine(px,100 - py,px,100 - py + 10);
    }

    if(maze[currentX][currentY].west == absent){
        Brain.Screen.setPenColor(blue);
        Brain.Screen.drawLine(px,100 - py,px,100 - py + 10);
    }


    // red tile
    if(maze[currentX][currentY].tileType == tile_red){
        Brain.Screen.setPenColor(red);
        Brain.Screen.drawRectangle(
            px + 2,
            100 - py + 2,
            6,
            6
        );
    }


  //robot pos
    Brain.Screen.setPenColor(green);
    Brain.Screen.drawCircle(px + 5,100 - py + 5,3);
}


void mazeSetup(){
    maze[0][0].visited = 1;
    maze[0][0].west = present;
    maze[0][0].south = present;
    maze[0][0].tileType = tile_white;

    // for(int i = 0; i < 10; i++){
    //     maze[0][i].east = unk;
    //     maze[i][0].south = unk;
    //     maze[9][i].west = unk;
    //     maze[i][9].north = unk;
    // }


   
    if(renderMap && mapOutline){
        
        Brain.Screen.setPenColor(white);
        Brain.Screen.drawLine(100, 0, 100, 100); // west wall
        Brain.Screen.drawLine(0,0, 100,0); //south wall 
        Brain.Screen.drawLine(0,0, 0, 100); // east wall 
        Brain.Screen.drawLine(0, 100, 100, 100); // north wall 


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

void printIner(){
    if(!renderMap){
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    Brain.Screen.print("%6.2f", iner.heading());
    Brain.Screen.print("TC: %d", turnCount);
    wait(100, msec);
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    }
}


void turn(int angle){
    drive.setTurnVelocity(TURN_SPEED_FAST, percent);
    drive.turnToHeading(angle, degrees);
    printIner();
    drive.setTurnVelocity(TURN_SPEED_SLOW, percent);
    drive.turnToHeading(angle, degrees);
    turnCount++;
    printIner();
    if(wallSquare){
    dTotal = 0;
    for(int i = 0; i < BUFFER_SIZE; i++){
                dstRding[i]  =  dSens.objectDistance(mm);
                dTotal += dstRding[i];
            }
            cDistance = dTotal / BUFFER_SIZE;
            double cRemainder = fmod(cDistance, 400);

        drive.setDriveVelocity(DRIVE_SPEED_SLOW, percent);

        if(cRemainder > GOOD_WALL_DIST){
            drive.driveFor(forward,(cRemainder - GOOD_WALL_DIST), mm);
            
        }

        if(cRemainder < GOOD_WALL_DIST){
            drive.driveFor(reverse,(GOOD_WALL_DIST - cRemainder), mm);
            
        }
    }

    drive.setDriveVelocity(DRIVE_SPEED, percent);



   // double actual = iner.heading();
    //double difference = fmod((double(angle) - actual) + 540.0, 360.0) - 180.0;

  //  int count = 0;
   // while(abs(difference) > 0.01){
   //     drive.setTurnVelocity(1, percent);
  //      drive.turnFor(difference, degrees);

   //     actual = iner.heading();
   //     difference = fmod((double(angle) - actual) + 540.0, 360.0) - 180.0;
   //     count++;
  //      if(count > 20){
  //          break;
   //     }

        
 //   }
}



void calibrate(){

    iner.calibrate();
    while(iner.isCalibrating())

        wait(50, msec);

    if(!renderMap){
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    }
    updateMap();
}



bool alignCheck(){
    if(!renderMap){
        Brain.Screen.print("aligning lol");
    }
    bool alignment = true;
        dTotal = 0;
        turn(270);

        for(int i = 0; i < BUFFER_SIZE; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            dTotal += dstRding[i];
        }
        align[0] = dTotal / BUFFER_SIZE;


        turn(180);
        dTotal = 0;
        for(int i = 0; i < BUFFER_SIZE; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            dTotal += dstRding[i];
        }
        align[1] = dTotal / BUFFER_SIZE;
    

    
        if((align[0] - align[1]) > 10){
            alignment = false;
        }
        turn(0);
        turn(0);
    
    if(!renderMap){
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    }
    updateMap();
    return alignment;

        
    
    
}



void startup(){
    tLed.on(orange);
    if(!renderMap){
        Brain.Screen.print("startup");
    }
    calibrate();
    mazeSetup();
    bool alignment = alignCheck();
    if(alignment){
        gState = STATE_IDLE;
    } else {
        gState = STATE_ERROR;
    }
}

void idle(){
    tLed.on(violet);
    if(!renderMap){
    Brain.Screen.print("idle");
    }
    while(WAIT_TIME){
        if(tLed.pressing()){
            gState = STATE_GATHER_INFO;
            break;
        }
    }
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


bool validCell(int x, int y){

    if(x < 0)
        return false;

    if(x >= MAZE_WIDTH)
        return false;

    if(y < 0)
        return false;

    if(y >= MAZE_HEIGHT)
        return false;

    return true;
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

void writeToWall(int x, int y, dir direction, wall set){
    if(validCell(currentX + x, currentY + y)){
        if(direction == north){
            maze[currentX + x][currentY + y].north = set;
        } else if(direction == east){
            maze[currentX + x][currentY + y].east = set;
        } else if(direction == south){
             maze[currentX + x][currentY + y].south = set;
        } else if(direction == west){
             maze[currentX + x][currentY + y].west = set;
        }
    }
}

void gatherInfo(){
    tLed.on(purple);
    if(!renderMap){
    Brain.Screen.print("gather");
    }
    
    maze[currentX][currentY].visited++;
    colourCheck();
    maze[currentX][currentY].tileType = currentTile;
    if(maze[currentX][currentY].tileType == tile_red){
        maze[currentX][currentY].visited = maze[currentX][currentY].visited + 999999;
    }

        updateMap();

    if(maze[currentX][currentY].north == unk){
        heading = 0;
        turn(heading);

        wallCheck();
        if(isWall == 0){
            writeToWall(0,0, north, absent);
            writeToWall(0,1, south, absent);               
            wallCheckLong();
            if(isWallLong == 0){
                writeToWall(0,1, north, absent);
                writeToWall(0,2, south, absent);
            } else {
                writeToWall(0,1, north, present);
                writeToWall(0,2, south, present);

                }

        } else {
            writeToWall(0,0,north,present);
            writeToWall(0,1, south, present);           
        }
    }

    updateMap();

    if(maze[currentX][currentY].east == unk){
        heading = 90;
        turn(heading);
        
        wallCheck();
        if(isWall == 0){
            writeToWall(0,0, east, absent);
            writeToWall(1, 0, west, absent);
            wallCheckLong();
            if(isWallLong == 0){
                writeToWall(1,0,east,absent);
                writeToWall(2,0,west,absent);
                } else {
                    writeToWall(1,0,east,present);
                    writeToWall(2,0,west,present);
                    }

        } else {
            writeToWall(0,0,east,present);
            writeToWall(1,0,west,present);

        }
    }

    updateMap();



    if(maze[currentX][currentY].south == unk){
        heading = 180;
        turn(heading);

        
        wallCheck();
        if(isWall == 0){
            writeToWall(0,0,south,absent);
            writeToWall(0, -1,north,absent);
            wallCheckLong();
            if(isWallLong == 0){
                writeToWall(0, -1, south,absent);
                writeToWall(0, -2, north, absent);
            } else {
                writeToWall(0,-1,south,present);
                writeToWall(0, -2, north, present);
            }
        } else {
            writeToWall(0,0,south,present);
            writeToWall(0, -1, north, present);

        }
    }

    updateMap();


    if(maze[currentX][currentY].west == unk){
        heading = 270;
        turn(heading);

        wallCheck();
        if(isWall == 0){
            writeToWall(0,0,west,absent);
            writeToWall(-1,0,east,absent);
            wallCheckLong();
            if(isWallLong == 0){
                writeToWall(-1,0,west,absent);
                writeToWall(-2, 0, east,absent);

            } else {
                writeToWall(-1,0,west,present);
                writeToWall(-2,0,east,present);
            }   

        } else {
            writeToWall(0,0,west,present);
            writeToWall(-1,0,east,present);

            
        }
    }
    updateMap();
    gState = STATE_DECIDE;

}

void decide(){
    tLed.on(blue);
    if(!renderMap){
    Brain.Screen.print("think");
    }
    
    if(maze[currentX][currentY].north == absent && maze[(currentX)][currentY + 1].visited == 0){
        heading = 0;
        currentY = currentY + 1;
    } else if(maze[currentX][currentY].east == absent && maze[currentX + 1][(currentY)].visited == 0) {
        heading = 90;
        currentX = currentX + 1;
    } else if(maze[currentX][currentY].south == absent && maze[(currentX)][currentY - 1].visited == 0){
        heading = 180;
        currentY = currentY - 1;
    } else if(maze[currentX][currentY].west == absent && maze[currentX - 1][(currentY)].visited == 0){
        heading = 270;
        currentX = currentX - 1;

        // we are stuck!!!


    } else if(maze[currentX][currentY].north == absent
        && maze[(currentX)][currentY + 1].visited < maze[(currentX)][currentY -1 ].visited
        && maze[(currentX)][currentY + 1].visited < maze[(currentX + 1)][currentY].visited 
        && maze[(currentX)][currentY + 1].visited <  maze[(currentX - 1)][currentY].visited)
        {
        heading = 0;
        currentY = currentY + 1;

    } else if(maze[currentX][currentY].east == absent 
        && maze[currentX + 1][(currentY)].visited < maze[currentX][(currentY -1 )].visited 
        && maze[currentX + 1][(currentY)].visited < maze[currentX - 1][(currentY)].visited) 
        {
        heading = 90;
        currentX = currentX + 1;

    } else if(maze[currentX][currentY].south == absent 
        && maze[(currentX)][currentY - 1].visited < maze[currentX -1][currentY].visited){
        heading = 180;
        currentY = currentY - 1;

    } else if(maze[currentX][currentY].west == absent){
        heading = 270;
        currentX = currentX - 1;
    }

    updateMap();
    gState = STATE_MOVE;
    
}

void move(){
    drive.setDriveVelocity(DRIVE_SPEED, percent);
    tLed.on(green);
    if(!renderMap){
    Brain.Screen.print("moving");
    }
    turn(heading);
    drive.driveFor(forward, 400, mm);
    dTotal = 0;
   
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
    // bool tempTest = false;
    // if(tempTest){

    // calibrate();
    // for(int i = 0; i < 30; i++){
    //     drive.setTurnVelocity(60, percent);
    //     printIner();
    //     drive.turnFor(rand() % (100 - (-100) + 1) + (-100), degrees);

    // }
    // drive.turnToHeading(0, degrees);
    // while(1){
    //     printIner();
    // }
    // }
    while(WAIT_TIME){
        if(!renderMap){
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
        }
        stateMachine();
    }
    }
    
