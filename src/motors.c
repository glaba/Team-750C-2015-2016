/** @file motors.c
 * @brief File for important motor functions
 *
 * This file contains the code for functions regarding motor status.
 * These functions are too complex to be defined as inline functions in the motors.h file.
 *
 * See the motors.h file for the basic movement functions.
 *
 * @see motors.h
 */

#include "main.h"

/** 
 * Sets the position of the transmission.
 * 
 * @param pos the position to set the transmission to.
 */
void transmissionSetPos(void *pos){
    int pot = (intptr_t) pos;
    printf("Target: %d\n", pot);
    if(analogRead(TRANSMISSION_POT) < pot) {
        while(analogRead(TRANSMISSION_POT) < pot){
            printf("Current: %d, Target: %d\n", analogRead(TRANSMISSION_POT), pot);
            transmission(sign(analogRead(TRANSMISSION_POT)-pot)*50);
            delay(20);
        }
    } else if(analogRead(TRANSMISSION_POT) > pot){
        while(analogRead(TRANSMISSION_POT) > pot){
            printf("Current: %d, Target: %d\n", analogRead(TRANSMISSION_POT), pot);
            transmission(sign(analogRead(TRANSMISSION_POT)-pot)*50);
            delay(20);
        }
    }
    printf("Task loop completed.\n");
    transmission(0);
}
