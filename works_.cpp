#include "vex.h"
#include "sensorsOfHubert.h"
#define GROUP_NUMBER 35
#define WALL_THRESHOLD 100
using namespace vex;
#define WAIT_TIME 10
#define ARY_SZ 10 // array size
#define S_RST_EGGS 1 //x
#define S_RST_WHY 1 //y
#define WHEEL_DIAM 63 //millimetres
#define WHEEL_CIRC (WHEEL_DIAM * 3.14159)
#define TRACK_WIDTH 143
#define WHEEL_BASE  112.5
#define GEAR_RATIO 1.0
#define MAZE_WIDTH 10
#define MAZE_HEIGHT 10
#define TURN_SPEED_FAST 90 
#define TURN_SPEED_SLOW 30
#define DRIVE_SPEED 90
#define DRIVE_SPEED_SLOW 40
#define GOOD_WALL_DIST 115
#define REQUIRED_PEOPLE 3
#define LIGHT_PERCENT
bool renderMap = true;
bool songs = true;
bool colourDebug = false;
bool mapOutline = false;
bool printRobotPos = false;
bool wallSquare = true;
bool backup = false;
bool backup2 = true;
int count = 0;
int person = 0;
int supplies = 0;
bool checkpoint = false;
int checkpointX;
int checkpointY;


void song(int number){
    // //0 - C
    // // 1 - D
    // // 2 - E
    // // 3 - F
    // // 4 - G
    // // 5 - A
    // // 6 - B

    if(number == 1){
        
        Brain.playNote(4,1);
        wait(250,msec);
        Brain.playNote(4,2);
        wait(250,msec);

        Brain.playNote(4,4, 250);
                wait(250,msec);

        Brain.playNote(4,2, 250);
                wait(250,msec);

        Brain.playNote(4,6, 250);  //give
                wait(250,msec);

        Brain.playNote(4,6,250);
                wait(300,msec); //you 

        Brain.playNote(4,5, 250);
                wait(450,msec); //up

        Brain.playNote(4,1, 250);
                wait(250,msec);

        Brain.playNote(4,2, 250);
                wait(250,msec);

        Brain.playNote(4,4, 250);
                wait(250,msec);

        Brain.playNote(4,2, 250);
                wait(250,msec);

        Brain.playNote(4,5, 250);
                wait(350,msec);

        Brain.playNote(4,5, 250);
                wait(250,msec);

        Brain.playNote(4,4, 250);
                wait(250,msec);

        Brain.playNote(4,1, 250);
                wait(250,msec);

        Brain.playNote(4,2, 250);
                wait(250,msec);

        Brain.playNote(4,4, 250);
                wait(250,msec);

        Brain.playNote(4,2, 250);
                wait(250,msec);



    }
}


motor lMotor = motor(PORT6, false);
motor rMotor = motor(PORT12, true);
inertial iner = inertial();
smartdrive drive = smartdrive(lMotor, rMotor, iner, WHEEL_CIRC, TRACK_WIDTH, WHEEL_BASE, mm, GEAR_RATIO);




double align[4];

double heading = 0;

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


state gState = STATE_STARTUP;
cell maze[MAZE_WIDTH][MAZE_HEIGHT];

void onPress() {
    if(gState == STATE_IDLE){
    } else {
        while(1){
            wait(10, msec);
        }
    }
}


// Draw the known part of the maze on the Brain screen.
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


    // blue tile
    if(maze[currentX][currentY].tileType == tile_blue){
        Brain.Screen.setPenColor(blue_green);
        Brain.Screen.drawRectangle(
            px + 2,
            100 - py + 2,
            6,
            6
        );
    }

    // green tile
    if(maze[currentX][currentY].tileType == tile_green){
        Brain.Screen.setPenColor(green);
        Brain.Screen.drawRectangle(
            px + 2,
            100 - py + 2,
            6,
            6
        );
    }

    if(maze[currentX][currentY].tileType == tile_yellow){
        Brain.Screen.setPenColor(yellow);
        Brain.Screen.drawRectangle(
            px + 2,
            100 - py + 2,
            6,
            6
        );
    }

    if(maze[currentX][currentY].tileType == tile_orange){
        Brain.Screen.setPenColor(orange);
        Brain.Screen.drawRectangle(
            px + 2,
            100 - py + 2,
            6,
            6
        );
    }

    if(maze[currentX][currentY].tileType == tile_white){
        Brain.Screen.setPenColor(white);
        Brain.Screen.drawRectangle(
            px + 2,
            100 - py + 2,
            6,
            6
        );
    }





  //robot pos

  if(printRobotPos){
    Brain.Screen.setPenColor(green);
    Brain.Screen.drawCircle(px + 5,100 - py + 5,3);
    }
}


// Set the starting cell and outer boundary.
void mazeSetup(){
    maze[0][0].visited = 1;
    maze[0][0].west = present;
    maze[0][0].south = present;
    maze[0][0].tileType = tile_white;


   
    if(renderMap && mapOutline){
        
        Brain.Screen.setPenColor(white);
        Brain.Screen.drawLine(100, 0, 100, 100); // west wall
        Brain.Screen.drawLine(0,0, 100,0); //south wall 
        Brain.Screen.drawLine(0,0, 0, 100); // east wall 
        Brain.Screen.drawLine(0, 100, 100, 100); // north wall 


    }
    
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


// Turn to a compass heading, then square up to the wall if enabled.
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

    if(cRemainder > GOOD_WALL_DIST && cDistance < 1500){
        drive.driveFor(forward,(cRemainder - GOOD_WALL_DIST), mm);
        
    }

    if(cRemainder < GOOD_WALL_DIST && cDistance < 1500){
        drive.driveFor(reverse,(GOOD_WALL_DIST - cRemainder), mm);
        
    }
    }

    drive.setDriveVelocity(DRIVE_SPEED, percent);



}

void turnFor(int angle){
    drive.setTurnVelocity(TURN_SPEED_FAST, percent);
    drive.turnFor(angle, degrees);
}



void calibrate(){

    iner.calibrate(8);
    while(iner.isCalibrating()){
        wait(50, msec);
        }

        

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
    iner.setHeading(0, degrees);
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
    if(currentX == 0 && currentY == 0 && person == REQUIRED_PEOPLE){
        song(1);
    }
    while(WAIT_TIME){
        if(tLed.pressing()){
            gState = STATE_GATHER_INFO;
            break;
        }
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

// Returns a large visit count for cells outside the maze.
// This prevents decide() from reading outside maze[][].
int visitedAt(int x, int y){
    if(!validCell(x, y))
        return 999999999;

    return maze[x][y].visited;
}

void markDeadEnd(){
    cell &c = maze[currentX][currentY];
    int walls = 0;
    int openings = 0;

    if(c.north == present) walls++; else if(c.north == absent) openings++;
    if(c.east  == present) walls++; else if(c.east  == absent) openings++;
    if(c.south == present) walls++; else if(c.south == absent) openings++;
    if(c.west  == present) walls++; else if(c.west  == absent) openings++;

    if(walls == 3 && openings == 1 && c.visited < 999999){
        c.visited += 999999;
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

#include "driveTo.h" //adds the new function!!!


// Scan the current tile and update nearby wall information.
void gatherInfo(){
    tLed.on(purple);
    if(!renderMap){
    Brain.Screen.print("gather");
    }
    
    maze[currentX][currentY].visited++;
    colourCheck();
    maze[currentX][currentY].tileType = currentTile;

    if(maze[currentX][currentY].tileType == tile_blue && maze[currentX][currentY].visited == 1){
    person++;
        tLed.on(blue);
        Brain.playSound(tada);
        wait(1000, msec);
        Brain.playSound(tada);
        tLed.off();
        wait(500, msec); // holy shit this one is a hazard
        Brain.playSound(tada);
        tLed.on(blue);
        wait(1000, msec);
        tLed.on(blue);
    }
    

    if(maze[currentX][currentY].tileType == tile_green){
        checkpoint = true;
        checkpointX = currentX; //yay i found a checkpoint
        checkpointY = currentY;
        tLed.on(green);
        Brain.playSound(tada);
        wait(1000, msec);
        Brain.playSound(tada);
        tLed.off();
        wait(500, msec); // holy shit this one is a hazard
        Brain.playSound(tada);
        tLed.on(green);
        wait(1000, msec);
        tLed.on(purple);
    }

        if(maze[currentX][currentY].tileType == tile_orange){
        supplies++;
          
        tLed.on(orange);
        Brain.playSound(tada);
        wait(1000, msec);
        Brain.playSound(tada);
        tLed.off();
        wait(500, msec); // holy shit this one is a hazard
        Brain.playSound(tada);
        tLed.on(orange);
        wait(1000, msec);
        tLed.on(purple);

    }


    if(maze[currentX][currentY].tileType == tile_red){
        maze[currentX][currentY].visited = maze[currentX][currentY].visited + 999999;
        
        tLed.on(red);
        Brain.playSound(alarm);
        wait(1000, msec);
        Brain.playSound(alarm);
        tLed.off();
        wait(500, msec); // holy shit this one is a hazard
        Brain.playSound(alarm);
        tLed.on(red);
        wait(1000, msec);
        tLed.on(purple);
    } else {
    

//east
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

//south
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


//west
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

//north
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







    }
    markDeadEnd();
    updateMap();
    gState = STATE_DECIDE;

}

int unexploredTiles(int x, int y){
    int nx = currentX + x;
    int ny = currentY + y;
    if(!validCell(nx, ny))
        return 0;

    int count = 0;
    if(maze[nx][ny].north == absent && validCell(nx, ny + 1) && maze[nx][ny + 1].visited == 0)
        count++;
    if(maze[nx][ny].east == absent && validCell(nx + 1, ny) && maze[nx + 1][ny].visited == 0)
        count++;
    if(maze[nx][ny].south == absent && validCell(nx, ny - 1) && maze[nx][ny - 1].visited == 0)
        count++;
    if(maze[nx][ny].west == absent && validCell(nx - 1, ny) && maze[nx - 1][ny].visited == 0)
        count++;

    if(maze[nx][ny].tileType == tile_red)
        count = 0;

    return count;
}

#include "decide.h"


// Physically move one maze cell in the chosen direction.
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

// Run one state-machine step.
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

void tileToPrint(tile cTile){
switch((int)cTile){
    case tile_black:
    Brain.Screen.print("black");
    break;

    case tile_red:
    Brain.Screen.print("red");
    break;

    case tile_pink:
    Brain.Screen.print("pink");
    break;

    case tile_orange:
    Brain.Screen.print("orange");
    break; 

    case tile_white:
    Brain.Screen.print("white");
    break; 


    case tile_yellow:
    Brain.Screen.print("yellow");
    break;

    case tile_green:
    Brain.Screen.print("green");
    break;

    case tile_blue:
    Brain.Screen.print("blue");
    break;

    case tile_error:
    Brain.Screen.print("error");
    break;
}
//return(text)
}


    int main(){
    opSens.setLightPower(50, vex::percentUnits::pct);

    while(WAIT_TIME){
        if(colourDebug){
            
            while(1){
                tile temp = tileCheck();
                tileToPrint(temp);
                Brain.Screen.newLine();
                printBrightness();
                Brain.Screen.newLine();
                printHue();
                wait(500, msec);
                Brain.Screen.clearScreen();
                Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);

            }
        }
        if(!renderMap){
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
        }

        stateMachine();
    }
    }
    
