#ifndef MOTORS_H_
#define MOTORS_H_

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
#define LEFT_MOTOR_TOP 2
#define LEFT_MOTOR_BOT 3

/**
 * Defines motor ports for the right side of the drivetrain.
 */
#define RIGHT_MOTOR_TOP 4
#define RIGHT_MOTOR_BOT 5

/** 
 * Moves the robot by setting the drive motor values.
 * 
 * @param spd the forward/backward speed value
 * @param turn the turning speed value
 */
inline void move(int spd, int turn){
    motorSet(LEFT_MOTOR_TOP, spd + turn);
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
#ifdef SHOOTER_HAS_THREE_MOTORS
#define NAUTILUS_SHOOTER_MOTOR_CENTER 10
#endif

/** 
 * Shoots balls from the shooter mechanism by setting the shooter motor values.
 * 
 * @param spd the speed to set the shooter motors
 */
inline void shoot(int spd){
    motorSet(NAUTILUS_SHOOTER_MOTOR_LEFT, spd);
    motorSet(NAUTILUS_SHOOTER_MOTOR_RIGHT, -spd);
#ifdef SHOOTER_HAS_THREE_MOTORS
    motorSet(NAUTILUS_SHOOTER_MOTOR_CENTER, -spd);
#endif /* SHOOTER_HAS_THREE_MOTORS */
}

/**
 * Defines motor ports for the intake mechanism.
 */
#define INTAKE_ROLLER_MOTOR 8

/** 
 * Intakes balls using the intake mechanism by setting the intake motor values.
 * 
 * @param spd the speed to set the intake motors
 */
inline void intake(int spd){
    motorSet(INTAKE_ROLLER_MOTOR, spd);
}

#define LIFT_DEPLOY 9

inline void deploy(int spd){
    motorSet(LIFT_DEPLOY, spd);
}

#ifndef SHOOTER_HAS_THREE_MOTORS
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
