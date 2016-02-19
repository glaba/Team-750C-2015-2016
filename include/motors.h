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
 * Defines motor port for the transmission to change between driving and lifting
 */
#define TRANSMISSION_MOTOR 1

/** 
 * Runs the transmission motor by setting the motor value.
 * 
 * @param spd the speed to run the transmission motor
 */
inline void transmission(int spd){
    motorSet(TRANSMISSION_MOTOR, spd);
}

/**
 * Moves the drive straight
 *
 * @param direction the direction to move (either forwards or backwards, which corresponds to 1 and -1)
 */
void moveStraight(int direction);

/** 
 * Sets the position of the transmission.
 * 
 * @param pos the position to set the transmission to.
 */
void transmissionSetPos(void *pos);

/** 
 * Changes the gear of the transmission to either driving or lifting.
 * Runs a task in a separate thread to change the gear.
 * 
 * @param gear the gear to change to
 */
inline void changeGear(int gear){
    taskCreate(transmissionSetPos, TASK_DEFAULT_STACK_SIZE, (void *) (intptr_t) gear, TASK_PRIORITY_DEFAULT);
}

/**
 * Defines motor ports for the left side of the drivetrain.
 */
#define LEFT_MOTOR_TOP 3
#define LEFT_MOTOR_BOT 5

/**
 * Defines motor ports for the right side of the drivetrain.
 */
#define RIGHT_MOTOR_TOP 2
#define RIGHT_MOTOR_BOT 4

/** 
 * Moves the robot by setting the drive motor values.
 * 
 * @param spd the forward/backward speed value
 * @param turn the turning speed value
 */
inline void move(int spd, int turn){
    motorSet(LEFT_MOTOR_TOP, -spd - turn);
    motorSet(LEFT_MOTOR_BOT, spd + turn);
    motorSet(RIGHT_MOTOR_TOP, -spd + turn);
    motorSet(RIGHT_MOTOR_BOT, -spd + turn);
}

/**
 * Defines motor ports for the nautilus gear shooting mechanism.
 */
#define SHOOTER_HAS_THREE_MOTORS

#define NAUTILUS_SHOOTER_MOTOR_LEFT 6
#define NAUTILUS_SHOOTER_MOTOR_RIGHT 7
#define NAUTILUS_SHOOTER_MOTOR_CENTER 8

/** 
 * Shoots balls from the shooter mechanism by setting the shooter motor values.
 * 
 * @param spd the speed to set the shooter motors
 */
inline void shoot(int spd){
    motorSet(NAUTILUS_SHOOTER_MOTOR_LEFT, -spd);
    motorSet(NAUTILUS_SHOOTER_MOTOR_RIGHT, spd);
    motorSet(NAUTILUS_SHOOTER_MOTOR_CENTER, spd);
}

/**
 * Defines motor ports for the intake mechanism.
 */
#define INTAKE_ROLLER_MOTOR 10

/** 
 * Intakes balls using the intake mechanism by setting the intake motor values.
 * 
 * @param spd the speed to set the intake motors
 */
inline void intake(int spd){
    motorSet(INTAKE_ROLLER_MOTOR, spd);
}

#define ROBOT_HAS_LIFT_DEPLOY_MOTOR 1

#ifdef ROBOT_HAS_LIFT_DEPLOY_MOTOR
#define LIFT_DEPLOY 9

inline void deploy(int spd){
    motorSet(LIFT_DEPLOY, spd);
}
#endif

#ifndef ROBOT_HAS_LIFT_DEPLOY_MOTOR
/**
 * Defines motor ports for the shooter angle adjustment motor.
 */
#define SHOOTER_ANGLE_MOTOR 10

/** 
 * Adjusts the shooter's angle by setting the motor.
 * 
 * @param spd the speed to set the angle adjustment motor
 */
inline void adjust(int spd){
    motorSet(SHOOTER_ANGLE_MOTOR, spd);
}
#endif /* SHOOTER_HAS_THREE_MOTORS */

#endif

