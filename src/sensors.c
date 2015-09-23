#include "main.h"

/** 
 * Object representing the encoder on the left side of the drivetrain.
 */
Encoder leftenc;

/** 
 * Object representing the encoder on the right side of the drivetrain.
 */
Encoder rightenc;

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
