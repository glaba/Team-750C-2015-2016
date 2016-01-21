/** @file opcontrol.h
 * @brief Header file for operator control definitions and prototypes
 *
 * This file contains definitions of the internal motor state variables
 * and prototypes for functions that record these variables and move the robot based on their value.
 */

#ifndef OPCONTROL_H
#define OPCONTROL_H

/**
 * Forward/backward speed of the drive motors.
 */
extern int spd;

/**
 * Turning speed of the drive motors.
 */
extern int turn;

/**
 * Speed of the shooter motors.
 */
extern int sht;

/**
 * Speed of the intake motor.
 */
extern int intk;

/**
 * Speed of the transmission motor.
 */
extern int trans;

/**
 * Speed of the lift deployment motor.
 */
extern int dep;

/**
 * Populates the motor state variables based on the joystick's current values.
 */
void recordJoyInfo();

/**
 * Moves the robot based on the motor state variables.
 */
void moveRobot();

#endif

