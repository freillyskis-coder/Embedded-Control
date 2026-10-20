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
// This is one implementation for creating a custom struct variable type named "homework_t"
typedef struct _homework_t {
    float average;      // The struct has a "field" for each value listed
    float stdev;        // in the "Homework Gradebook" table.
    uint8_t min;        // Note that the fields can be all different types
    uint8_t max;
    uint16_t subs;
} homework_t;

// Add global variables here as needed.
homework_t hw[5] = {
    {95.2, 10.8, 56, 100, 108}, //hw1
    {75.7, 12.6, 41, 100, 105}, //hw2
    {83.5, 15.9, 24, 100, 98},
    {91.1, 12.6, 44, 100, 95},
    {65.3, 27.3, 11, 100, 95}
};

homework_t hw1 = {95.2, 10.8, 56, 100, 108};
// Prototype. Add above main function, below the homework_t definition.
float remove_min_from_avg(homework_t *homework);

// Declaration. Add below main function
// This function calculates the average of a homework without the minimum grade.
// The function will return the value of the new average.
float remove_min_from_avg(homework_t *homework){
    float homework_sum = homework->average*homework->subs;
    homework_sum -= homework->min;
    return homework_sum/(homework->subs-1);
}

int main() {      /// Main Function ///
  
    // Add local variables here as needed.
    
    // We will always call the sysInit function first to set 
    // up the microcontroller for how we are going to use it.
    // Do not use printf() above this line
    sysInit();
    uint8_t i;
    /*
    // Place initialization code (or run-once) code here
    for(i=0; i < 5; i++){
        printf("Homework %u Stats\r\n"
           "    Average: %.2f\r\n"
           "  Std. Dev.: %.2f\r\n"
          "    Minimum: %u\r\n"
           "    Maximum: %u\r\n"
           "Submissions: %u\r\n",
           i+1,hw[i].average,hw[i].stdev,hw[i].min,
           hw[i].max,hw[i].subs);
    }      
    */
    float newHw1Avg = remove_min_from_avg(&hw1);
    printf("Homework 1 Average without min: %.2f\r\n", newHw1Avg);
    homework_t hw2 = hw[1];
    float newHw2Avg = remove_min_from_avg(&hw2);
    printf("Homework 2 Average without min: %.2f\r\n", newHw2Avg);
    float newHw2AvgArray = remove_min_from_avg(&hw[1]);
    printf("Homework 2 Average without min: %.2f\r\n", newHw2AvgArray);
    while(1) {  
        // Place code that runs continuously in here

    }   
}      /// End Main Function ///  

// Add function declarations here as needed

// Add interrupt functions last so they are easy to find
