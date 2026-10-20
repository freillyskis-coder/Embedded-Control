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
void TimerInit();
uint8_t UpdateCountdown();
// Add global variables here as needed.
Timers_TimerConfig timer;

uint8_t minutes,seconds,resets;


    int main() {      /// Main Function ///
  
    // Add local variables here as needed.
    uint32_t interrupt;
    seconds = 10;
    minutes = 1;
    // We will always call the sysInit function first to set 
    // up the microcontroller for how we are going to use it.
    // Do not use printf() above this line
    sysInit();
    TimerInit();
    GPIO_initDigitalOutput(GPIOA, GPIO_PIN0);
    GPIO_initDigitalOutput(GPIOB, GPIO_PIN13);
    
    // Place initialization code (or run-once) code here
    
    while(1) {  
        // Place code that runs continuously in here
        interrupt = Timers_getAllPendingInterrupts(TIMG8);
        if(interrupt & TIMER_INTSRC_ZERO){
            Timers_clearInterrupt( TIMG8 , TIMER_INTSRC_ZERO );
            resets++;
            GPIO_togglePins(GPIOB, GPIO_PIN13);
            if(resets == 100){ // change number here corresponding to 1 s
                GPIO_togglePins(GPIOA, GPIO_PIN0);// Toggle LED1 (P1.0)
                if(UpdateCountdown()){
                    printf("the timer has ended");
                    Timers_stopTimer(TIMG8);
                }
                resets = 0;
            }
            
        }
        
        
    }   
}      /// End Main Function ///  

// Add function declarations here as needed
void TimerInit(){
    timer.mode = TIMER_MODE_PERIODIC_DOWN;
    timer.clksrc = TIMER_CLOCK_BUSCLK;
    timer.clkdivratio = 0;
    timer.clkprescale = 39;
    timer.period = 7999;
    Timers_initTimer(TIMG8,&timer);
    Timers_startTimer(TIMG8);
}

uint8_t UpdateCountdown(){
    if(seconds == 0 & minutes == 0)
        return 1;
    if(seconds == 0){
        seconds = 60;
        minutes --;
    }
    seconds --;
    printf("%02u:%02u\r",minutes,seconds);
    return 0;
}
// Add interrupt functions last so they are easy to find
