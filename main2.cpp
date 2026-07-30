
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       carterchlo                                                */
/*    Created:      16/07/2026, 14:08:29                                      */
/*    Description:  IQ2 project                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"
#define GROUP_NUMBER 35
using namespace vex;

// A global instance of vex::brain used for printing to the IQ2 brain screen
vex::brain       Brain;

// define your global instances of motors and other devices here


void onPress() {
    Brain.Screen.clearLine
    Brain.Screen.print("Hello!");
}

int main() {
touchled TouchLED1 = touchled(PORT10);
bumper Bumper1 = bumper(PORT11);
bool tPressed = TouchLED1.pressing();
bool bPressed = Bumper1.pressing();
int tCount = 0;
int bCount = 0;
vex::color ledColor = vex::color(000,000,000);
TouchLED1.pressed(onPress);

while(1){
    //bool pressedOld = pressed
    bool oldPressed = bPressed;
    bPressed = Bumper1.pressing();
    tPressed =  TouchLED1.pressing();
    
    if(bPressed && !oldPressed){
        Brain.Screen.clearLine();
        bCount++;
        Brain.Screen.print("Pressed");
        Brain.Screen.print(" %d",bCount);


    } else {
        if(!bPressed && oldPressed){
        Brain.Screen.clearLine();
        Brain.Screen.print("Not pressed");
        }

    }

    if(tPressed && !bPressed){
        ledColor = vex::color(255,000,000);
        TouchLED1.on(ledColor, 100);

    } else {
        if(!tPressed && bPressed){
            ledColor = vex::color(000,000,255);
            TouchLED1.on(ledColor, 100);

            } else {
                if(tPressed && bPressed){
                    ledColor = vex::color(255,000,255);
                    TouchLED1.on(ledColor, 100); 
            } else {
                TouchLED1.off();
            }
        }
    }
    wait(30, msec);
    }
}

