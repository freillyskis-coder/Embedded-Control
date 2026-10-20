////////////////////////////////////////////////////////////////////////
//** ENGR-2350 Template Project 
//** NAME: Finnian Reilly
//** RIN: 662123514
//** TEMPLATE VERSION: v.99 (2026-01-05)
//** This is the base project for several activities and labs throughout
//** the course.  The outline provided below isn't necessarily *required*
//** by a C program; however, this format is required within ENGR-2350
//** to ease debugging/grading by the staff.
////////////////////////////////////////////////////////////////////////

// We'll always add this include statement. This basically takes the
// code contained within the "engr2350_mspm0.h" file and adds it here.
#include "engr2350_mspm0.h"

// Add function prototypes here as needed.
void gpioInit();

// Add global variables here as needed.


int main() {      /// Main Function ///
  
    // Add local variables here as needed.
    int let;
    uint8_t count;
    // We will always call the sysInit function first to set 
    // up the microcontroller for how we are going to use it.
    // Do not use printf() above this line
    sysInit();
    gpioInit();
    // Place initialization code (or run-once) code here
    
    while(1) {  
        // Place code that runs continuously in here
        let = getchar();
        if(let == 'f'){
            GPIO_clearPins(GPIOB, GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN6 | GPIO_PIN7);
            delay_cycles(320);
            GPIO_setPins(GPIOB, GPIO_PIN3 | GPIO_PIN7);
        }
        else if (let == 'r'){
            GPIO_clearPins(GPIOB, GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN6 | GPIO_PIN7);
            delay_cycles(320);
            GPIO_setPins(GPIOB, GPIO_PIN6 | GPIO_PIN2);
        }
        else if(let == 's'){
            GPIO_clearPins(GPIOB, GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN6 | GPIO_PIN7);
            delay_cycles(320);

        }
        printf("line %u\n",count++);
    }   
}      /// End Main Function ///  

// Add function declarations here as needed
void gpioInit(){
    GPIO_initDigitalOutput(GPIOB, GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN6 | GPIO_PIN7);
    GPIO_clearPins(GPIOB, GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN6 | GPIO_PIN7);
}
// Add interrupt functions last so they are easy to find
