////////////////////////////////////////////////////////////////////////
//** ENGR-2350 Template Project 
//** NAME: Reilly
//** RIN: Finnian
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


// Add global variables here as needed.


int main() {      /// Main Function ///
  
    // Add local variables here as needed.

    // We will always call the sysInit function first to set 
    // up the microcontroller for how we are going to use it.
    // Do not use printf() above this line
    sysInit();

    // Place initialization code (or run-once) code here
    uint32_t array1[] = {1,2,3,4,5,6,7,8,9,10};
    uint32_t array2[15];
    uint32_t array3[10][15];
    // Place code that runs continuously in here
        uint8_t i,j;         // Loop count variable
        for(i=0;i<10;i++){ // Loop through the indices of the array
            printf("Element %u is:\t%u\r\n",i,array1[i]);
        
        for(i=0;i<15;i++){ // Loop through the indices of the array
            array2[i] = rand() % 21;
            printf("Element %u is:\t%u\r\n",i,array2[i]);
        }
        
        for(i=0;i<10;i++){  // Outer loop
           for(j=0;j<15;j++){  // Inner loop
                array3[i][j] = array1[i]*array2[j];// In here we can use i and j to loop through elements,
                // where i and j will be the row/column index of the 2d matrix
            }
        }
        // This portion prints one of the input arrays column header (horizontal)
        printf("\n\n\t");   // Move down a couple lines and skip the first "column"
        for(i=0;i<10;i++){
            printf("\t%u",i+1); // Print each element in the array, one each column (tab)
        }
        printf("\r\n"); // move to the next line to print the table

        // This portion prints the other input array as the row headers (vertical)
        // and the multiplication table (2D array)
        for(i=0;i<15;i++){
            printf("\t%u",i+1); // Print the i-th element of the input array
            for(j=0;j<10;j++){
                printf("\t%u",array3[j][i]); // print all the elements of the i-th row for the table
            }
            printf("\r\n"); // move to next line
        }
    while(1) {  
        
    }
    }   
}      /// End Main Function ///  

// Add function declarations here as needed

// Add interrupt functions last so they are easy to find
