
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
#define WALL_THRESHOLD 100
using namespace vex;

// A global instance of vex::brain used for printing to the IQ2 brain screen
vex::brain       Brain;

// define your global instances of motors and other devices here
#define WAIT_TIME 100
#define ARY_SZ 10 // array size
#define S_RST_EGGS 1 //x
#define S_RST_WHY 1 //y

int main(){
    double rding; // the reading
    double hueShortForHue; // hue is short for hue
    double brtns; //brightness
    distance dSens = distance(PORT5);
    optical opSens = optical(PORT1);
    double dstRding[ARY_SZ]; // distance reading array of specific size
    double hueShortForHueRding[ARY_SZ];
    double brtnsRding[ARY_SZ];
    opSens.setLight(ledState::on);
    touchled tLed = touchled(PORT10);
    tLed.on(white);

   
    while(1){

        double dTotal = 0;
        double dAverage = 0;
        double oBrtnsTotal = 0;
        double oBrtnsAverage = 0;
        double oHueShortForHueTotal = 0;
        double oHueShortForHueAverage = 0;
        for(int i = 0; i < ARY_SZ; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            dTotal += dstRding[i];

            hueShortForHueRding[i] = opSens.hue();
            oHueShortForHueTotal += hueShortForHueRding[i];

            brtnsRding[i] = opSens.brightness();
            oBrtnsTotal += brtnsRding[i];

        }

        
        dAverage = dTotal/ARY_SZ;
        oBrtnsAverage = oBrtnsTotal/ARY_SZ;
        oHueShortForHueAverage = oHueShortForHueTotal/ARY_SZ;

        Brain.Screen.print("Dist: %.1f", dAverage);
        Brain.Screen.newLine();
        Brain.Screen.print("%.1f", oHueShortForHueAverage);
        Brain.Screen.print(", %.1f", oBrtnsAverage);
        if(dAverage > WALL_THRESHOLD){
            tLed.setColor(green);
        } else {
            tLed.setColor(red);
        }
        
        wait(WAIT_TIME, msec);
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    }




















    // tasks 1 2 3
    while(1){

        double total = 0;
        double average = 0;
        for(int i = 0; i < ARY_SZ; i++){
            dstRding[i]  =  dSens.objectDistance(mm);
            total += dstRding[i];
            wait(1, msec);

        }
        average = total/ARY_SZ;
        Brain.Screen.print("Distance: %.2f", average);
        Brain.Screen.newLine();

        if(average > WALL_THRESHOLD){
            Brain.Screen.print("clear lol");
        } else {
            Brain.Screen.print("Ruh roh");
        }
        
        wait(WAIT_TIME, msec);
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(S_RST_EGGS, S_RST_WHY);
    }


     while(1){
        hueShortForHue = opSens.hue();
        brtns = opSens.brightness();
        Brain.Screen.print("%.2f", hueShortForHue);
        Brain.Screen.print(", %.2f", brtns);
        wait(WAIT_TIME, msec);
        Brain.Screen.clearLine();
    
    }


}