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
 * Speed of the strafing motor.
 */
extern int strafe;

/**
 * Speed of the angle change motor.
 */
extern int ang;

/**
 * Speed of the lift motors.
 */
extern int lift;

/**
 * Populates the motor state variables based on the joystick's current values.
 */
void recordJoyInfo();

/**
 * Moves the robot based on the motor state variables.
 */
void moveRobot();

/** 
 * Faces the robot towards the desired gyroscope angle by turning it.
 * This function implements a simple PID control loop in order to correct for error.
 * 
 * @param target the desired robot angle
 * @return the drive turning speed value
 */
int targetNet(int target);

/** 
 * Resets the gyroscope PID variables so it can be used a second time.
 */
void resetGyroVariables();

#endif

