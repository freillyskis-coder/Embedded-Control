////////////////////////////////////////////////////////////////////////
//** ENGR-2350 Activity: Stopwatch Template Project
//** NAME: Finnian Reilly
//** RIN: 662123514
////////////////////////////////////////////////////////////////////////

#include "engr2350_mspm0.h"

// Function Prototypes
void GPIOInit();
void TimerInit();
void UpdateTime(uint8_t * time_arry); // Take time array and add 1/100 s to it.
void TIMG7_IRQHandler();
// **ACTIVITY** Add needed function prototypes

// Global Variables
Timers_TimerConfig timer;
uint8_t total_time[4]; // array to keep track of [tenths of seconds,seconds,minutes,hours]
uint8_t lap_time[4]; // Current lap time (previous lap not stored, only printed once)
uint8_t lap_num = 1; // Current lap number, start on lap 1
uint8_t print_flag = 1; // Flag to note that time should be printed on screen.
                        // We start this at 1 to print time at 0 (instead of leaving blank)
uint8_t counter = 0; // For toggling the LED
uint8_t timerRunning = 0;
uint8_t newLap = 0;
// **ACTIVITY** Add remaining variables needed here

int main() {      /// Main Function ///
    
    sysInit();
    GPIOInit();
    TimerInit();
    //Timers_startTimer(TIMG7);
    printf("\r\nLap #\tLap Time\tTotal Time \r\n"); // Print header
    while(1) {  
        // Place code that runs continuously in here
        
        // Check if new lap is requested. **ACTIVITY**: Will need to replace 0
        if(newLap) {
            putchar('\n'); // Make a new line in the printing for next lap
            // **ACTIVITY** Reset flag, reset lap time array and increment lap number
            newLap = 0;
            lap_num ++;
            for(uint8_t i = 0; i < 4 ; i++){
                lap_time[i] = 0;
            }
        }
        // Check if time to update print. This is complete, no changes needed
        if( print_flag ) {
            printf("\r%6u\t%02u:%02u:%02u.%02u\t%02u:%02u:%02u.%02u\t",lap_num,lap_time[3],lap_time[2],lap_time[1],lap_time[0]
                                                              ,total_time[3],total_time[2],total_time[1],total_time[0]);
            print_flag = 0;
        }
    }
}      /// End Main Function ///  

// Add function declarations here as needed
void GPIOInit( ) {
    // **ACTIVITY**
    // Configure pin for PA0 for LED1 to blink
    GPIO_initDigitalOutput(GPIOA, GPIO_PIN0);
    

    // Configure used GPIO - PA18,PB21 for Devboard switches S1 and S2, OR
                       //  - PA7,PB25 for RPI-RSLK bumpers BMP1 and BMP6
    // The pins should be configured as inputs and should trigger
    // the associated interrupt ON PRESS ONLY.
    GPIO_initDigitalInput(GPIOA, GPIO_PIN7); //PA7
    GPIO_enableInterrupt(GPIOA, GPIO_PIN7);
    GPIO_setInternalResistor(GPIOA, GPIO_PIN7, GPIO_PULL_UP);
    GPIO_setInterruptPolarity(GPIOA, GPIO_PIN7, GPIO_INT_EDGE_FALL);

    GPIO_initDigitalInput(GPIOB, GPIO_PIN25);//PB25
    GPIO_enableInterrupt(GPIOB, GPIO_PIN25);
    GPIO_setInternalResistor(GPIOB, GPIO_PIN25, GPIO_PULL_UP);
    GPIO_setInterruptPolarity(GPIOB, GPIO_PIN25, GPIO_INT_EDGE_FALL);

    NVIC_EnableIRQ(GPIO_INT_IRQn);//enabling GPIO Interrupts in CPU
}

void TimerInit(){
    // **ACTIVITY** Configure a timer to run at 10 Hz and trigger reset interrupt (most copied from Activity 7)
    timer.mode = TIMER_MODE_PERIODIC_DOWN;
    timer.clksrc = TIMER_CLOCK_BUSCLK;
    timer.clkdivratio = TIMER_CLOCK_DIVIDE_5;
    timer.clkprescale = 7;
    timer.period = 7999;
    Timers_initTimer(TIMG7,&timer);
    Timers_enableInterrupt(TIMG7, TIMER_INTSRC_ZERO);
    NVIC_EnableIRQ(TIMG7_INT_IRQn);
}

// Use this function to update the time arrays. Note that the argument requested is
// the time array itself (arrays are by definition pointers, no & needed).
void UpdateTime(uint8_t * time_arry){
    time_arry[0]++;  // Increment hundredths of seconds
    if(time_arry[0] == 100){  // If a whole second has passed...
        time_arry[0] = 0;    // Reset tenths of seconds
        time_arry[1]++;      // And increment seconds
        if(time_arry[1] == 60){ // If a minute has passed...
            time_arry[1] = 0;   // Reset seconds
            time_arry[2]++;     // Increment minutes
            if(time_arry[2] == 60){  // and so on...
                time_arry[2] = 0;
                time_arry[3]++;
                if(time_arry[3] == 24){
                    time_arry[3] = 0;
                }
            }
        }
    }
}

// **ACTIVITY** Interrupt for the timer
void TIMG7_IRQHandler(){
    uint32_t spot = Timers_getActiveInterrupt(TIMG7);
    Timers_clearInterrupt(TIMG7, spot);
    counter++;
    UpdateTime(total_time);
    UpdateTime(lap_time);
    if(counter>=100){
        counter = 0;
        GPIO_togglePins(GPIOA, GPIO_PIN0);
    }
    print_flag = 1;
}

// **ACTIVITY** Interrupt for the switches/bumpers
void GPIO_IRQHandler(){
    // This example assumes that PA10 and PA11 are configured for
// triggering an interrupt on a GPIO_INT_EDGE_RISE edge
// This code would be within the GPIO_IRQHandler() function

    // First, we'll debouce the press to avoid multiple triggers for one press:
delay_cycles(320e3); // 10 ms delay (32 MHz clock)

    // Next: get the list of pins that may have triggered the interrupt
uint32_t active_pins = GPIO_getPendingInterrupts(GPIOA) | GPIO_getPendingInterrupts(GPIOB);

    // Check to see if PA7 interrupt is active
    if(active_pins & GPIO_PIN7){
        // Clear the GPIO_PIN7 interrupt
        GPIO_clearInterrupt(GPIOA,GPIO_PIN7);
        //printf("hello");
        // It is possible that bouncing from the a falling-edge transition can also
        // trigger the interrupt. The if statement below is ensuring that the pin's
        // value after debouncing (above) is HIGH; which verifies that the transition
        // was in fact a rising-edge, not falling-edge
        if(!GPIO_readPins(GPIOA,GPIO_PIN7)){
            // PA7 press detected, do whatever is needed
            if(timerRunning){
                timerRunning = 0;
                Timers_stopTimer(TIMG7);
            }
            else {
                timerRunning = 1;
                Timers_startTimer(TIMG7);
            }
        }
    }
        // Repeat for GPIO_PIN25 (PB25).
    if(active_pins & GPIO_PIN25){
        GPIO_clearInterrupt(GPIOB,GPIO_PIN25);
        //printf("hellooo");
        if(!GPIO_readPins(GPIOB,GPIO_PIN25)){
            // PA25 Press detected, do whatever is needed
            if(!timerRunning){
                lap_num = 1;
                print_flag = 1;
                for(uint8_t i = 0; i < 4 ; i++){
                    lap_time[i] = 0;
                    total_time[i] = 0;
                }
            }
            else {
                newLap = 1;
            }
        }
    }
}

