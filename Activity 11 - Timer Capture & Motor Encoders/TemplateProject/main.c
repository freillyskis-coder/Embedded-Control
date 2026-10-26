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
void TIMG8_IRQHandler();

// Add global variables here as needed.

uint32_t enc_total_events;
int32_t enc_counts_track;
int32_t enc_counts;
uint8_t enc_flag;

int main() {      /// Main Function ///
    
    // Add local variables here as needed.

    // We will always call the sysInit function first to set 
    // up the microcontroller for how we are going to use it.
    // Do not use printf() above this line
    sysInit();
    gpioInit();
    timerInit();
    //int __enable_interrupts();
    
    // Place initialization code (or run-once) code here
    printf("\r\nDistance\tEnc_Counts\tDelta T\t\tAng.Speed\tLin.Speed\r\n"); // Column headers
    //printf("hello");
    while(1) {  
        // Place code that runs continuously in here
        //printf("hat");
        if(enc_flag){ // Check to see if capture occurred
            enc_flag = 0; // reset capture flag
            uint16_t distance = (uint16_t)(enc_total_events * (70.0f * 3.14159f / 360.0f)); // Calculate distance travelled in mm using enc_total_events
            float delta_t = (float)enc_counts / 32000000.0f; // Calculate the time between previous and current event using enc_counts
            float speed_rpm = (1.0f / 360.0f) * (1.0f / delta_t) * 60.0f; // Calculate the instantaneous wheel speed in rpm
            float speed_mm = speed_rpm/60*3.1415*70 ; // Calculate the instantaneous car speed in mm/s
            
            printf("%5u mm\t%8u\t%7.1f ms\t%5.2f rpm\t%5.2f mm/s\r\n",distance,enc_counts,delta_t*1000,speed_rpm,speed_mm);
        }
        
    }   
}      /// End Main Function ///  

// Add function declarations here as needed
void gpioInit() {
    GPIO_initDigitalInput(GPIOB, GPIO_PIN10);
    GPIO_initPeripheralFunction(GPIOB, GPIO_PIN10, GPIO_PB10_PF_TIMG8_C0); //TIMG8 C0
    NVIC_EnableIRQ(GPIO_INT_IRQn);
    
    GPIO_initDigitalOutput(GPIOB, GPIO_PIN22);
}

void timerInit() {
    Timers_TimerConfig tim;
    tim.mode = TIMER_MODE_PERIODIC_UP;
    tim.clkdivratio = TIMER_CLOCK_DIVIDE_1;
    tim.clksrc = TIMER_CLOCK_BUSCLK;
    tim.period = 65535;
    tim.clkprescale = 0;
    
    Timers_initTimer(TIMG8, &tim);

    Timers_CaptureConfig config;
    config.ccrn = TIMER_CCR_CCR0;
    config.edge = TIMER_CCR_EDGE_RISE;
    config.inputSel = TIMER_CCR_INPUT_CCPn;
    config.invertInput = 0;
    Timers_initCapture(TIMG8, &config);

    Timers_enableInterrupt(TIMG8, TIMER_INTSRC_LOAD | TIMER_INTSRC_CCR0_UP);
    
    NVIC_EnableIRQ(TIMG8_INT_IRQn);

    Timers_startTimer(TIMG8);
}

// Add interrupt functions last so they are easy to find

void TIMG8_IRQHandler(){
    uint32_t interrupt = Timers_getPendingInterrupts(TIMG8);
    //printf("car");
    //GPIO_togglePins(GPIOB, GPIO_PIN22);
    if(interrupt & TIMER_INTSRC_LOAD){
        Timers_clearInterrupt(TIMG8, TIMER_INTSRC_LOAD);
        enc_counts_track += 65536;
    }
    if(interrupt == TIMER_INTSRC_CCR0_UP){
        Timers_clearInterrupt(TIMG8, TIMER_INTSRC_CCR0_UP);
        uint16_t ccrVal = Timers_getCCRValue(TIMG8, TIMER_CCR_CCR0);
        
        enc_total_events += 1;
        enc_counts = enc_counts_track + ccrVal;
        enc_counts_track = -1*(ccrVal);
        enc_flag = 1;
    }
}
