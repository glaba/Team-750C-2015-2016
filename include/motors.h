/** @file motors.h
 * @brief Header file for important motor functions and definitions
 *
 * This file contains the code for functions and definitions regarding motor status.
 * Mainly, this file defines the motor ports for each mechanism.
 * It also defines certain motor-related constants.
 * Lastly, basic movement functions are defined as inline in this file.
 *
 * Some functions are too complex to be defined as inline functions in the motors.h file.
 * See the motors.c file for these more complicated movement functions.
 *
 * @see motors.c
 */

#ifndef MOTORS_H_
#define MOTORS_H_

/**
 * Defines maximum motor speed value.
 */
#define MOTOR_MAX 127

/**
 * Defines motor minimum speed value.
 */
#define MOTOR_MIN -127

/**
 * Moves the drive straight.
 *
 * Uses the encoders to make a PID control loop, keeping the drive straight.
 *
 * @param direction the direction to move (either forwards or backwards, which corresponds to 1 and -1)
 */
void moveStraight(int direction);


/**
 * Defines motor ports for the left side of the drivetrain.
 */
#define LEFT_MOTOR 2

/**
 * Defines motor ports for the right side of the drivetrain.
 */
#define RIGHT_MOTOR 3

/**
 * Defines motor port for the strafing motor of the drivetrain.
 */
#define STRAFE_MOTOR 4

/** 
 * Moves the robot by setting the drive motor values.
 * 
 * @param spd the forward/backward speed value
 * @param turn the turning speed value
 */
inline void move(int spd, int turn, int strafe){
    motorSet(LEFT_MOTOR, -spd + turn);
    motorSet(RIGHT_MOTOR, -spd - turn);
    motorSet(STRAFE_MOTOR, strafe);
}

/** 
 * Moves the robot by setting the drive motor values by side.
 * 
 * @param l the left motor speed
 * @param r the right motor speed
 */
inline void move_lr(int l, int r){
    motorSet(LEFT_MOTOR, l);
    motorSet(RIGHT_MOTOR, r);
}

/**
 * Defines the motor configuration of the nautilus shooter.
 *
 * Represents the fact that the nautilus shooter has three motors.
 */
#define SHOOTER_HAS_THREE_MOTORS

/**
 * Defines the motor port for the left nautilus gear motor.
 */
#define NAUTILUS_SHOOTER_MOTOR_LEFT 6

/**
 * Defines the motor port for the right nautilus gear motor.
 */
#define NAUTILUS_SHOOTER_MOTOR_RIGHT 7

/**
 * Defines the motor port for the center nautilus gear motor.
 */
#define NAUTILUS_SHOOTER_MOTOR_CENTER 8

/** 
 * Shoots balls from the shooter mechanism by setting the shooter motor values.
 * 
 * @param spd the speed to set the shooter motors
 */
inline void shoot(int spd){
    motorSet(NAUTILUS_SHOOTER_MOTOR_LEFT, spd);
    motorSet(NAUTILUS_SHOOTER_MOTOR_RIGHT, spd);
    motorSet(NAUTILUS_SHOOTER_MOTOR_CENTER, -spd);
}

/**
 * Defines motor ports for the intake mechanism.
 */
#define INTAKE_ROLLER_MOTOR 5

/** 
 * Intakes balls using the intake mechanism by setting the intake motor values.
 * 
 * @param spd the speed to set the intake motors
 */
inline void intake(int spd){
    motorSet(INTAKE_ROLLER_MOTOR, -spd);
}

/**
 * Defines motor ports for the shooter angle adjustment motor.
 */
#define SHOOTER_ANGLE_MOTOR 9

/** 
 * Adjusts the shooter's angle by setting the angle change motor speed.
 * 
 * @param spd the speed to set the angle adjustment motor
 */
inline void adjust(int spd){
    motorSet(SHOOTER_ANGLE_MOTOR, -spd);
}

/**
 * Defines motor ports for the left side of the lift.
 */
#define LIFT_MOTOR_LEFT 10

/**
 * Defines motor ports for the right side of the lift.
 */
#define LIFT_MOTOR_RIGHT 1

/**
 * Sets the speed of the lift motors.
 *
 * @param spd the speed to set the lift motors to
 */
inline void lift_raw(int spd){
    motorSet(LIFT_MOTOR_LEFT, spd);
    motorSet(LIFT_MOTOR_RIGHT, -spd);
}

#endif

