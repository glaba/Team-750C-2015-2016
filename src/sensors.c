/** @file sensors.c
 * @brief File for important sensor declarations and functions
 *
 * This file contains the code for declarations and functions regarding sensors.
 * The definitions contained herein define objects representing more complex sensors.
 * The functions contained herein are too complex to be defined as inline functions in the sensors.h file.
 *
 * See the sensors.h file for the basic sensory definitions and functions.
 *
 * @see sensors.h
 */

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
 * Object representing the gyroscope.
 */
Gyro gyro;

/**
 * Object representing the ultrasonic sensor.
 */
Ultrasonic sonar;

