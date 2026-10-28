////////////////////////////////////////////////////////////////////////
//** ENGR-2350 Template Project 
//** NAME: Finnian Reilly and Marcus Gazda
//** RIN: 662123514 & 
//** TEMPLATE VERSION: v.99 (2026-01-05)
//** This is the base project for several activities and labs throughout
//** the course.  The outline provided below isn't necessarily *required*
//** by a C program; however, this format is required within ENGR-2350
//** to ease debugging/grading by the staff.
////////////////////////////////////////////////////////////////////////

//LAB 3

// We'll always add this include statement. This basically takes the
// code contained within the "engr2350_mspm0.h" file and adds it here.
#include "engr2350_mspm0.h"

// Add function prototypes here as needed.

void gpioInit();
void timerInit();
void TIMG8_IRQHandler();

// Add global variables here as needed.

uint32_t sumVarTracker; //number of events for averaging
uint32_t encoderSum; //sum for averaging

int main() {      /// Main Function ///
  
    // Add local variables here as needed.

    // We will always call the sysInit function first to set 
    // up the microcontroller for how we are going to use it.
    // Do not use printf() above this line
    sysInit();
    gpioInit();
    timerInit();
    //__enable_interrupts();
    // Place initialization code (or run-once) code here
    
    while(1) {  
        // Place code that runs continuously in here
        printf("h");
    }   
}      /// End Main Function ///  

// Add function declarations here as needed

void gpioInit() {
    GPIO_initDigitalInput(GPIOB, GPIO_PIN10 | GPIO_PIN11); //encoder input
    GPIO_initDigitalOutput(GPIOB, GPIO_PIN6 | GPIO_PIN7); //PWM as output
    GPIO_initDigitalOutput(GPIOB, GPIO_PIN22); // Blue LED

    //bumper buttons
    GPIO_initDigitalInput(GPIOA, GPIO_PIN7 | GPIO_PIN14); //PB1,2
    GPIO_initDigitalInput(GPIOB, GPIO_PIN23 | GPIO_PIN25); //BMP3,4,5,6
    GPIO_setInternalResistor(GPIOA, GPIO_PIN7 | GPIO_PIN14, GPIO_PULL_UP);
    GPIO_setInternalResistor(GPIOB, GPIO_PIN23 | GPIO_PIN25, GPIO_PULL_UP);
    
    GPIO_initDigitalOutput(GPIOA, GPIO_PIN0); //LED1 I think

    //peripheral funcs
    GPIO_initPeripheralFunction(GPIOB, GPIO_PIN11, 5); //right encoder
    GPIO_initPeripheralFunction(GPIOB, GPIO_PIN10, 2); //left encoder
    GPIO_initPeripheralFunction(GPIOB, GPIO_PIN6 | GPIO_PIN7, 5); //PWMs
    NVIC_EnableIRQ(GPIO_INT_IRQn);

    //motor setup code
    GPIO_setPins(GPIOB, GPIO_PIN15 | GPIO_PIN16); // enables motors
    GPIO_clearPins(GPIOB, GPIO_PIN0 | GPIO_PIN8); // sets DIR to forward (low)
}

void timerInit() {

    //PWM timer
    Timers_TimerConfig tim1;
    tim1.mode = TIMER_MODE_PERIODIC_UP;
    tim1.clkdivratio = TIMER_CLOCK_DIVIDE_1;
    tim1.clksrc = TIMER_CLOCK_BUSCLK;
    tim1.period = 800;
    tim1.clkprescale = 0;
    
    Timers_initTimer(TIMG8, &tim1); 
    //left PWM
    Timers_CompareConfig leftPWM;
    leftPWM.ccrn = TIMER_CCR_CCR1;
    leftPWM.action = TIMER_CCR_ACTION_ZERO_SET | TIMER_CCR_ACTION_UPCOMPARE_CLEAR;
    leftPWM.value = 200; //initial value
    leftPWM.invertOutput = 0;
    Timers_initCompare(TIMG8, &leftPWM);
    //right PWM
    Timers_CompareConfig rightPWM;
    rightPWM.ccrn = TIMER_CCR_CCR0;
    rightPWM.action = TIMER_CCR_ACTION_ZERO_SET | TIMER_CCR_ACTION_UPCOMPARE_CLEAR;
    rightPWM.value = 200; //initial value
    rightPWM.invertOutput = 0;
    Timers_initCompare(TIMG8, &rightPWM);
    
    Timers_startTimer(TIMG8);
    
    Timers_TimerConfig encoder;
    encoder.mode = TIMER_MODE_PERIODIC_UP;
    encoder.clkdivratio = TIMER_CLOCK_DIVIDE_1;
    encoder.clksrc = TIMER_CLOCK_BUSCLK;
    encoder.period = 65535;
    encoder.clkprescale = 0;
    
    Timers_initTimer(TIMG0, &encoder);//left enc
    Timers_initTimer(TIMG6, &encoder);//right enc
    
    Timers_CaptureConfig leftEncoder;
    leftEncoder.ccrn = TIMER_CCR_CCR0;
    leftEncoder.edge = TIMER_CCR_EDGE_RISE;
    leftEncoder.inputSel = TIMER_CCR_INPUT_CCPn;
    leftEncoder.invertInput = 0;
    Timers_initCapture(TIMG0, &leftEncoder);
    Timers_enableInterrupt(TIMG0, TIMER_INTSRC_LOAD | TIMER_INTSRC_CCR0_UP);
    NVIC_EnableIRQ(TIMG0_INT_IRQn);
    Timers_startTimer(TIMG0);
    
    Timers_CaptureConfig rightEncoder;
    rightEncoder.ccrn = TIMER_CCR_CCR1;
    rightEncoder.edge = TIMER_CCR_EDGE_RISE;
    rightEncoder.inputSel = TIMER_CCR_INPUT_CCPn;
    rightEncoder.invertInput = 0;
    Timers_initCapture(TIMG6, &rightEncoder);
    Timers_enableInterrupt(TIMG6, TIMER_INTSRC_LOAD | TIMER_INTSRC_CCR1_UP);
    NVIC_EnableIRQ(TIMG6_INT_IRQn);
    Timers_startTimer(TIMG6);
}

// Add interrupt functions last so they are easy to find

void TIMG0_IRQHandler(){
    uint32_t interrupt = Timers_getPendingInterrupts(TIMG0);
    printf("car");
    GPIO_togglePins(GPIOB, GPIO_PIN22);
    if(interrupt & TIMER_INTSRC_LOAD){
        Timers_clearInterrupt(TIMG0, TIMER_INTSRC_LOAD);
        //enc_counts_track += 65536;
    }
    if(interrupt & TIMER_INTSRC_CCR0_UP){
        Timers_clearInterrupt(TIMG0, TIMER_INTSRC_CCR0_UP);
        uint16_t ccrVal = Timers_getCCRValue(TIMG0, TIMER_CCR_CCR0);
        
        printf("%u \n",ccrVal);
    }
}

void TIMG6_IRQHandler() {
    uint32_t interrupt = Timers_getPendingInterrupts(TIMG6);
    if(interrupt & TIMER_INTSRC_LOAD){
        Timers_clearInterrupt(TIMG6, TIMER_INTSRC_LOAD);
        // track right resets 
    }
    if(interrupt & TIMER_INTSRC_CCR1_UP){ 
        Timers_clearInterrupt(TIMG6, TIMER_INTSRC_CCR1_UP);
        uint16_t ccrVal = Timers_getCCRValue(TIMG6, TIMER_CCR_CCR1);
        // process right encoder event
    }
}