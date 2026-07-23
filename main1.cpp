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


int main() {
	int robot = 2;
    double pi = 3.14;
    Brain.Screen.print("Hello from");
    Brain.Screen.newLine(); 
    Brain.Screen.print("Chloe and Evelynn");
    Brain.Screen.newLine(); 
    Brain.Screen.print("I'm in Group %d",GROUP_NUMBER);
    Brain.Screen.newLine(); 
    Brain.Screen.print("%d",robot);
    Brain.Screen.newLine(); 
    Brain.Screen.print("%.2f",pi);
    // Create a touchled instance in Port 10
    touchled TouchLED10 = touchled(PORT10);
        TouchLED10.on(50,50,50,90);
        vex::color ledColor = vex::color(255,255,000);
        TouchLED10.setBrightness(10);
        TouchLED10.setBlink(ledColor,0.5,0.5);
    
    while(1) {
        
        // Allow other tasks to run
        this_thread::sleep_for(10);
    }


}
