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
 * Speed of the intake motors.
 */
extern int intk;

/**
 * Speed of the transmission motors.
 */
extern int trans;

/**
 * Populates the motor state variables based on the joystick's current values.
 */
void recordJoyInfo();

/**
 * Moves the robot based on the motor state variables.
 */
void moveRobot();

#endif

