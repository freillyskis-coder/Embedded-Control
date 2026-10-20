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
void timerInit();

// Add global variables here as needed.


int main() {      /// Main Function ///
  
    // Add local variables here as needed.

    // We will always call the sysInit function first to set 
    // up the microcontroller for how we are going to use it.
    // Do not use printf() above this line
    sysInit();
    gpioInit();
    timerInit();
    // Place initialization code (or run-once) code here
    int16_t leftPWM = 80; 
    int16_t rightPWM = 80; 
    while(1) {  
        // Place code that runs continuously in here
        if(!GPIO_readPins(GPIOA, GPIO_PIN7)){
            delay_cycles(320e3);
            while(!GPIO_readPins(GPIOA, GPIO_PIN7)); // Wait until released
            delay_cycles(320e3);
            leftPWM += 80;
            if(leftPWM >= 720){
                leftPWM = 720;
            }
            Timers_setCCRValue(TIMG8, TIMER_CCR_CCR1, leftPWM); //ccr1 = left motor PWM
        }
        else if(!GPIO_readPins(GPIOA, GPIO_PIN14)){
            delay_cycles(320e3);
            while(!GPIO_readPins(GPIOA, GPIO_PIN14)); // Wait until released
            delay_cycles(320e3);
            leftPWM -= 80;
            if(leftPWM <= 80){
                leftPWM = 80;
            }
            Timers_setCCRValue(TIMG8, TIMER_CCR_CCR1, leftPWM); //ccr1 = left motor PWM
        }
        else if(!GPIO_readPins(GPIOB, GPIO_PIN5)){
            delay_cycles(320e3);
            while(!GPIO_readPins(GPIOB, GPIO_PIN5)); // Wait until released
            delay_cycles(320e3);
            GPIO_togglePins(GPIOB, GPIO_PIN0); //PB0 is right motor direction
        }
        else if(!GPIO_readPins(GPIOB, GPIO_PIN21)){
            delay_cycles(320e3);
            while(!GPIO_readPins(GPIOB, GPIO_PIN21)); // Wait until released
            delay_cycles(320e3);
            rightPWM += 80;
            if(rightPWM >= 720){
                rightPWM = 720;
            }
            Timers_setCCRValue(TIMG8, TIMER_CCR_CCR0, rightPWM);
        }
        else if(!GPIO_readPins(GPIOB, GPIO_PIN23)){
            delay_cycles(320e3);
            while(!GPIO_readPins(GPIOB, GPIO_PIN23)); // Wait until released
            delay_cycles(320e3);
            rightPWM -= 80;
            if(rightPWM <= 80){
                rightPWM = 80;
            }
            Timers_setCCRValue(TIMG8, TIMER_CCR_CCR0, rightPWM);
        }
        else if(!GPIO_readPins(GPIOB, GPIO_PIN25)){
            delay_cycles(320e3);
            while(!GPIO_readPins(GPIOB, GPIO_PIN25)); // Wait until released
            delay_cycles(320e3);
            GPIO_togglePins(GPIOB, GPIO_PIN8); //PB0 is right motor direction
        }
        printf("leftPWM: %d   rightPWM: %d\n", leftPWM, rightPWM);
    }   
}      /// End Main Function ///  

// Add function declarations here as needed
void gpioInit(){
    GPIO_initDigitalInput(GPIOA, GPIO_PIN7 | GPIO_PIN14);
    GPIO_setInternalResistor(GPIOA, GPIO_PIN7 | GPIO_PIN14, GPIO_PULL_UP); //BMP1 and 2
    
    GPIO_initDigitalInput(GPIOB, GPIO_PIN5 | GPIO_PIN21 | GPIO_PIN23 | GPIO_PIN25);
    GPIO_setInternalResistor(GPIOB, GPIO_PIN5 | GPIO_PIN21 | GPIO_PIN23 | GPIO_PIN25, GPIO_PULL_UP); //BMP 3,4,5,6
    
    GPIO_initDigitalOutput(GPIOB, GPIO_PIN0 | GPIO_PIN8 | GPIO_PIN15 | GPIO_PIN16);
    GPIO_initDigitalOutput(GPIOB, GPIO_PIN6 | GPIO_PIN7); // PWM
    GPIO_initPeripheralFunction(GPIOB, GPIO_PIN6 | GPIO_PIN7, 5); //CCR 0 and 1 for TIMG84

    GPIO_setPins(GPIOB, GPIO_PIN15 | GPIO_PIN16); // enables motors
    GPIO_clearPins(GPIOB, GPIO_PIN0 | GPIO_PIN8); // sets DIR to forward (low)
}

void timerInit(){
    Timers_TimerConfig tim;
    tim.mode = TIMER_MODE_PERIODIC_UP;
    tim.clkdivratio = TIMER_CLOCK_DIVIDE_1;
    tim.clksrc = TIMER_CLOCK_BUSCLK;
    tim.clkprescale = 0;
    tim.period = 800;
    Timers_initTimer(TIMG8, &tim);
    
    Timers_CompareConfig config;
    config.ccrn = TIMER_CCR_CCR0 | TIMER_CCR_CCR1;
    config.action = TIMER_CCR_ACTION_ZERO_SET | TIMER_CCR_ACTION_UPCOMPARE_CLEAR;
    config.value = 80;
    config.invertOutput = 0;
    Timers_initCompare(TIMG8, &config);

    Timers_startTimer(TIMG8);
}
// Add interrupt functions last so they are easy to find
