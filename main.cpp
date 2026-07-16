/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       carterchlo                                                */
/*    Created:      16/07/2026, 14:08:29                                      */
/*    Description:  IQ2 project                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"

using namespace vex;

// A global instance of vex::brain used for printing to the IQ2 brain screen
vex::brain       Brain;

// define your global instances of motors and other devices here


int main() {
	
    Brain.Screen.print("Hello from");
    Brain.Screen.newLine();
    Brain.Screen.print("Chloe and Evelynn");

   
    while(1) {
        
        // Allow other tasks to run
        this_thread::sleep_for(10);
    }
}