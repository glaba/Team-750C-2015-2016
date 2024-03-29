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

float enc_integral = 0;
float enc_derivative = 0;
float enc_previous_error = 0;

/**
 * Moves the drive straight by using the encoders to make a PID correction loop.
 *
 * @param speed the speed to move forward at
 */
void moveStraight(int speed) {
    speed = constrain(speed, -110, 110);

    float error = encoderGet(rightenc) - encoderGet(leftenc);

    enc_integral += error * 20;
    enc_derivative = (error-enc_previous_error)/20.0;

    move_lr(speed, (int)((float)speed - ((float)speed)/((float)110.0) * (float)(ENCODER_KP * error + ENCODER_KI * enc_integral - ENCODER_KD * enc_derivative)));
    
    enc_previous_error = error;
}

void resetEncoderVariables(){
    clearDriveEncoders();
    enc_integral = 0;
    enc_derivative = 0;
    enc_previous_error = 0;
}

