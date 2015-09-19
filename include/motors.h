#ifndef MOTORS_H_
#define MOTORS_H_

#include <API.h>

#define LEFT_MOTOR_TOP 2
#define LEFT_MOTOR_MID 3
#define LEFT_MOTOR_BOT 4

#define RIGHT_MOTOR_TOP 5
#define RIGHT_MOTOR_MID 6
#define RIGHT_MOTOR_BOT 7

inline void move(int spd, int turn){
    motorSet(LEFT_MOTOR_TOP, -spd - turn);
    motorSet(LEFT_MOTOR_MID, spd + turn);
    motorSet(LEFT_MOTOR_BOT, -spd - turn);
    motorSet(RIGHT_MOTOR_TOP, spd - turn);
    motorSet(RIGHT_MOTOR_MID, -spd + turn);
    motorSet(RIGHT_MOTOR_BOT, spd - turn);
}

#define SHOOTER_MOTOR_LEFT 8
#define SHOOTER_MOTOR_RIGHT 9

inline void shoot(int spd){
    motorSet(SHOOTER_MOTOR_LEFT, spd);
    motorSet(SHOOTER_MOTOR_RIGHT, spd);
}


#define TRANSMISSION_MOTOR 1

inline void transmission(int spd){
    motorSet(TRANSMISSION_MOTOR, spd);
}

void transmissionSetPos(void *pos);

inline void changeGear(int gear){
    taskCreate(transmissionSetPos, TASK_DEFAULT_STACK_SIZE, (void *) (intptr_t) gear, TASK_PRIORITY_DEFAULT);
}

#endif
