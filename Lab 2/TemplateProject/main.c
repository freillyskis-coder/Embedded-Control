////////////////////////////////////////////////////////////////////////
//** ENGR-2350 Lab 2
//** NAME: Finnian Reilly, Marcus Gazda
//** RIN: 662123514
//** TEMPLATE VERSION: v.99 (2026-01-05)
//** Section: 1
//** Side: B
//** Seat: 32
//** This is the base project for several activities and labs throughout
//** the course.  The outline provided below isn't necessarily *required*
//** by a C program; however, this format is required within ENGR-2350
//** to ease debugging/grading by the staff.
////////////////////////////////////////////////////////////////////////

// We'll always add this include statement. This basically takes the
// code contained within the "engr2350_mspm0.h" file and adds it here.
#include "engr2350_mspm0.h"

// Add function prototypes here as needed.
void GPIOinit();
void TimerInit();
void TIMG7_IRQHandler();
void getGPIO();
void lightUpRGB(uint8_t);
uint8_t gamePlay();
// Add global variables here as needed.
Timers_TimerConfig timer;
uint16_t timeLen; //in milliseconds
uint8_t rounds; //# of current round
uint8_t biledGreen, biledRed; //vars to tell if we should flash biLED
uint8_t start; //tells us if we should start game
uint8_t pb1,bumper1, bumper2, bumper3, bumper4, bumper5, bumper6;
uint8_t bumper;
uint16_t countdown;
uint8_t isButtonPressed;
uint8_t overTime;
uint8_t sequence[10];
int main() {      /// Main Function ///
  
    // Add local variables here as needed.
    
    // We will always call the sysInit function first to set 
    // up the microcontroller for how we are going to use it.
    // Do not use printf() above this line
    sysInit();
    GPIOinit();
    TimerInit();
    // Place initialization code (or run-once) code here
    start = 0;
    Timers_startTimer(TIMG7);
    printf("\r\nThis game is called Simon. A series of flashes will occur on the RGB LED. You will have to enter the correct sequence of bumper pushes one after another. If it takes you longer than 3 seconds in between pushes or you incorrectly enter the sequence, you lose! Press PB1 to start.\r\n");
    while(1) {  
        // Place code that runs continuously in here
        getGPIO();
        if(pb1){
            start = 1;
            biledRed = 0;
            biledGreen = 0;
            GPIO_clearPins(GPIOB, GPIO_PIN2 | GPIO_PIN3);
        }
        lightUpRGB(bumper);
        if(start){ 
            uint8_t winLose = gamePlay();
            
            if(winLose == 1){
                printf("\r\n\nyou suck lol. you messed up on round %u", rounds);
                biledRed = 1;
            }
            else if(winLose == 0){
                printf("\r\n\nyour the slowest. you took too long on round %u", rounds);
                biledRed = 1;
            }
            else if(winLose == 2){
                printf("\r\n\nCONGRATULATIONS! YOU WON!");
                biledGreen = 1;
            }
        }
        start = 0;
        if(countdown >= 250){
            if(biledGreen){
                GPIO_togglePins(GPIOB, GPIO_PIN2);
            }
            else if(biledRed){
                GPIO_togglePins(GPIOB, GPIO_PIN3);
            }
            countdown = 0;
        }
    }
}   
    /// End Main Function ///  

// Add function declarations here as needed
void GPIOinit(){
    GPIO_initDigitalInput(GPIOA, GPIO_PIN12); //Pushbutton
    GPIO_initDigitalOutput(GPIOB, GPIO_PIN2 | GPIO_PIN3); //biLED
    GPIO_initDigitalOutput(GPIOB, GPIO_PIN22 | GPIO_PIN26 | GPIO_PIN27); //RGB
    GPIO_initDigitalInput(GPIOA, GPIO_PIN7| GPIO_PIN14); //Bumper 1 + 2
    GPIO_initDigitalInput(GPIOB, GPIO_PIN5 | GPIO_PIN21 | GPIO_PIN23 | GPIO_PIN25); //Bumper 3-6
    
    GPIO_setInternalResistor(GPIOA, GPIO_PIN7 | GPIO_PIN14, GPIO_PULL_UP);
    GPIO_setInternalResistor(GPIOB, GPIO_PIN5 | GPIO_PIN21 | GPIO_PIN23 | GPIO_PIN25, GPIO_PULL_UP);
}

void TimerInit(){
    timer.mode = TIMER_MODE_PERIODIC_UP;
    timer.clksrc = TIMER_CLOCK_BUSCLK;
    timer.clkdivratio = TIMER_CLOCK_DIVIDE_4;
    timer.clkprescale = 9; // goes to 10 for a Ndiv of 40
    timer.period = 799;
    Timers_initTimer(TIMG7, &timer);
    Timers_enableInterrupt(TIMG7, TIMER_INTSRC_LOAD);
    NVIC_EnableIRQ(TIMG7_INT_IRQn);
}
//reads all pins of GPIO inputs and sets correct variables
void getGPIO(){
    pb1 = GPIO_readPins(GPIOA, GPIO_PIN12) != 0;
    bumper1 = GPIO_readPins(GPIOA, GPIO_PIN7) == 0;
    bumper2 = GPIO_readPins(GPIOA, GPIO_PIN14) == 0;
    bumper3 = GPIO_readPins(GPIOB, GPIO_PIN5) == 0;
    bumper4 = GPIO_readPins(GPIOB, GPIO_PIN21) == 0;
    bumper5 = GPIO_readPins(GPIOB, GPIO_PIN23) == 0;
    bumper6 = GPIO_readPins(GPIOB, GPIO_PIN25) == 0;
    if(bumper1){
        bumper = 1;
    }
    else if(bumper2){
        bumper = 2;
    }
    else if(bumper3){
        bumper = 3;
    }
    else if(bumper4){
        bumper = 4;
    }
    else if(bumper5){
        bumper = 5;
    }
    else if(bumper6){
        bumper = 6;
    }
    else{
        bumper = 0;
    }
    delay_cycles(320000);
}

uint8_t gamePlay(){
    timeLen = 500;
    uint8_t i = 0;
    for(i = 0; i<20; i++){ // generate array
        sequence[i] = (rand()%6) + 1;
    }
    for(rounds = 1; rounds <= 20; rounds++){  // one for each round
        for(i = 0; i < rounds; i++){ //lighting up the colors
            lightUpRGB(sequence[i]);
            countdown = 0;
            while(countdown<timeLen){}
            lightUpRGB(0);
            countdown = 0;
            while(countdown<timeLen){}
        }
        for(i = 0; i < rounds; i++){ //looking for inputs for one color
            countdown = 0;
            while(!bumper){
                getGPIO();
                if(countdown >= 3000){
                    return 0; // timeout
                }
            }
            lightUpRGB(bumper);
            if(sequence[i] != bumper){
                return 1; // 1 means incorrect bumper press
            }
            while(bumper){
                getGPIO();
                if(countdown >= 3000){
                    return 0; // timeout
                }
            }
            
        }
        printf("\r\nCongrats! You completed round %u",rounds);
        lightUpRGB(0);
        countdown = 0;
        while(countdown <= timeLen);
        timeLen *= 0.9;
    }
    return 2;
    
}



void lightUpRGB(uint8_t bump){
    if(bump == 1){//red
        GPIO_setPins(GPIOB, GPIO_PIN26);
        GPIO_clearPins(GPIOB, GPIO_PIN22 | GPIO_PIN27);
    }
    else if(bump == 2){//green
        GPIO_setPins(GPIOB, GPIO_PIN27);
        GPIO_clearPins(GPIOB, GPIO_PIN22 | GPIO_PIN26);
    }
    else if(bump == 3){ //blue
        GPIO_setPins(GPIOB, GPIO_PIN22);
        GPIO_clearPins(GPIOB, GPIO_PIN27 | GPIO_PIN26);
    }
    else if(bump == 4){ //cyan
        GPIO_setPins(GPIOB, GPIO_PIN27 | GPIO_PIN22);
        GPIO_clearPins(GPIOB, GPIO_PIN26);
    }
    else if(bump == 5){ //yellow
        GPIO_setPins(GPIOB, GPIO_PIN27 | GPIO_PIN26);
        GPIO_clearPins(GPIOB, GPIO_PIN22);
    }
    else if(bump == 6){ //magenta
        GPIO_setPins(GPIOB, GPIO_PIN22 | GPIO_PIN26);
        GPIO_clearPins(GPIOB, GPIO_PIN27);
    }
    else if(bump == 0){ //clear
        GPIO_clearPins(GPIOB, GPIO_PIN22 | GPIO_PIN26 | GPIO_PIN27);
    }

}

// Add interrupt functions last so they are easy to find
void TIMG7_IRQHandler(){
    Timers_clearInterrupt(TIMG7, TIMER_INTSRC_LOAD);
    countdown++;
}
