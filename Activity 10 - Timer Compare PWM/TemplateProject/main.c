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
void GPIOInit();
void TimerInit();

// Add global variables here as needed.
float duty = 0.5;
float counts = 16384;
uint16_t duty_length;

uint8_t gpioA7, gpioA14, gpioB23, gpioB25;
int main() {      /// Main Function ///
  
    // Add local variables here as needed.

    // We will always call the sysInit function first to set 
    // up the microcontroller for how we are going to use it.
    // Do not use printf() above this line
    sysInit();
    GPIOInit();
    duty_length = duty*counts;
    TimerInit();
    // Place initialization code (or run-once) code here
    Timers_startTimer(TIMG8);
    
    GPIO_setPins(GPIOA, GPIO_PIN7 | GPIO_PIN14);
    GPIO_setPins(GPIOB, GPIO_PIN23 | GPIO_PIN25);
    printf("BMP1:%u\n", !GPIO_readPins(GPIOA, GPIO_PIN7));
    printf("BMP2:%u\n", !GPIO_readPins(GPIOA, GPIO_PIN14));
    printf("BMP5:%u\n", !GPIO_readPins(GPIOB, GPIO_PIN23));
    printf("BMP6:%u\n", !GPIO_readPins(GPIOB, GPIO_PIN25));
    printf("\n");

    duty_length = duty*counts;
    while(1) {  
        // Place code that runs continuously in here
        
        gpioA7 = !GPIO_readPins(GPIOA, GPIO_PIN7);
        gpioA14 = !GPIO_readPins(GPIOA, GPIO_PIN14);
        gpioB23 = !GPIO_readPins(GPIOB, GPIO_PIN23);
        gpioB25 = !GPIO_readPins(GPIOB, GPIO_PIN25);
        
        if(gpioA7){ // BMP1 Pressed 
            delay_cycles(320e3);
            //printf("bmp1\n");
            while(!GPIO_readPins(GPIOA, GPIO_PIN7)); // Wait until released
            delay_cycles(320e3);
        
            counts = counts * 0.9;
            duty_length = duty * counts;
            Timers_setPeriod(TIMG8, (uint16_t)counts);
            Timers_setCCRValue(TIMG8, TIMER_CCR_CCR1, duty_length);
        }
        else if(gpioA14){ // BMP2 Pressed
            delay_cycles(320e3);
            //printf("bmp2\n");
            while(!GPIO_readPins(GPIOA, GPIO_PIN14)); // Wait until released
            delay_cycles(320e3);
        
            counts = counts * 1.1;
            duty_length = duty * counts;
            Timers_setPeriod(TIMG8, (uint16_t)counts);
            Timers_setCCRValue(TIMG8, TIMER_CCR_CCR1, duty_length);
        }
        else if(gpioB23){ // BMP5 Pressed
            delay_cycles(320e3);
            //printf("bmp5\n");
            while(!GPIO_readPins(GPIOB, GPIO_PIN23)); // Wait until released
            delay_cycles(320e3);
        
            duty = duty * 1.1;
            if(duty > 1.0) duty = 1.0;
            duty_length = duty * counts;
            Timers_setCCRValue(TIMG8, TIMER_CCR_CCR1, duty_length);
        }
        else if(gpioB25){ // BMP6 Pressed
            delay_cycles(320e3);
            while(!GPIO_readPins(GPIOB, GPIO_PIN25)); // Wait until released
            delay_cycles(320e3);
            //printf("bmp6\n");
            duty = duty * 0.9;
            if(duty < 0.0) duty = 0.0;
            duty_length = duty * counts;
            Timers_setCCRValue(TIMG8, TIMER_CCR_CCR1, duty_length);
        }
        
        printf("Duty Cycle: %.2f  counts: %.0f  duty length: %u  BMP1: %u   BMP2: %u  BMP5: %u   BMP6: %u\r\n", duty, counts, duty_length, gpioA7, gpioA14, gpioB23, gpioB25);
    }   
}      /// End Main Function ///  

// Add function declarations here as needed
void GPIOInit(){
    GPIO_initDigitalInput(GPIOA, GPIO_PIN7 | GPIO_PIN14);
    GPIO_setInternalResistor(GPIOA, GPIO_PIN7 | GPIO_PIN14, GPIO_PULL_UP);
    
    GPIO_initDigitalInput(GPIOB, GPIO_PIN23 | GPIO_PIN25);
    GPIO_setInternalResistor(GPIOB, GPIO_PIN23 | GPIO_PIN25, GPIO_PULL_UP);
    GPIO_initDigitalOutput(GPIOA, GPIO_PIN0);

    GPIO_initPeripheralFunction(GPIOA, GPIO_PIN0, 6);
}

void TimerInit(){
    Timers_TimerConfig tim;
    tim.mode = TIMER_MODE_PERIODIC_UP;
    tim.clkdivratio = TIMER_CLOCK_DIVIDE_2;
    tim.clksrc = TIMER_CLOCK_LFCLK;
    tim.period = counts;
    tim.clkprescale = 0;
    Timers_initTimer(TIMG8, &tim);
    Timers_CompareConfig config;
    config.ccrn = TIMER_CCR_CCR1;
    config.action = TIMER_CCR_ACTION_ZERO_SET | TIMER_CCR_ACTION_UPCOMPARE_CLEAR;
    config.value = (duty_length);
    config.invertOutput = 1;

    Timers_initCompare(TIMG8, &config);
}

// Add interrupt functions last so they are easy to find
