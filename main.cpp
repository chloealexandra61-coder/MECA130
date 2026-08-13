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
#define WHEEL_DIAM 63 //millimetres
#define WHEEL_CIRC (3.14159265 * WHEEL_DIAM)
#define TRACK_WIDTH 143
#define WHEEL_BASE  112.5
#define GEAR_RATIO 1.0


int count = 0;
int ind = 0;

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


void calibrate(){

    iner.calibrate();
    Brain.Screen.print("calibrating lol");
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


int main(){

    calibrate();
    int i = 1;
    drive.setHeading(0, degrees);
    bool alignment = alignCheck();
    if(alignment){
        Brain.Screen.print("align good");
    } else {
        Brain.Screen.print("align bad");
    }
while(alignment){
    double heading = (90 * i) % 360;
    drive.driveFor(forward, 300, mm);
    drive.turnToHeading(heading, degrees);
    i++;
    }
}


