/** @file autonroutines.c
 * @brief File for hard-coded autonomous routines
 *
 * This file contains code for hard-coded autonomous routines that use sensors.
 */

#include "main.h"

/**
 * Runs a programming skills routine using sensors rather than the autonomous recorder.
 * Starts in the left side of the field shooting into the closer goal.
 */
void runHardCodedProgrammingSkills() {
    int numShots = 0;
    bool shooterLimitPressed = UNPRESSED;
    shoot(-127);
    while (numShots <= 32) {
        if (shooterLimitPressed == PRESSED && digitalRead(SHOOTER_LIMIT) == UNPRESSED) {
            numShots++;
        }
        shooterLimitPressed = digitalRead(SHOOTER_LIMIT);
        delay(20);
    }
    shoot(0);
    int integral = 0;
    int prev_error = 0;
    while (gyroGet(gyro) % ROTATION_DEG != -90 - CLOSE_GOAL_ANGLE /* turning right */) {
        int error = (-90 - CLOSE_GOAL_ANGLE) - gyroGet(gyro) % ROTATION_DEG;
        integral += error * 20;
        int derivative = (error-prev_error)/20;
        move(0, error * GYRO_KP + integral * GYRO_KI + derivative * GYRO_KD);
        prev_error = error;
    }
    gyroReset(gyro);
    while (ultrasonicGet(sonar) < DISTANCE_TO_OTHER_SIDE - 30) {
        move(127, 0);
    }
    while (ultrasonicGet(sonar) < DISTANCE_TO_OTHER_SIDE) {
        move(64, 0);
    }
    integral = 0;
    prev_error = 0;
    while (gyroGet(gyro) % ROTATION_DEG != 90 /* turning left */) {
        int error = 90 - gyroGet(gyro) % ROTATION_DEG;
        integral += error * 20;
        int derivative = (error-prev_error)/20;
        move(0, error * GYRO_KP + integral * GYRO_KI + derivative * GYRO_KD);
        prev_error = error;
    }

    /*numShots = 0;*/
    shoot(127);
    delay(20);
    shoot(0);
    /*shooterLimitPressed = UNPRESSED;*/
    taskCreate(playSpeaker, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
    shoot(-127);
    /*while (numShots <= 32) {*/
        /*if (shooterLimitPressed == PRESSED && digitalRead(SHOOTER_LIMIT) == UNPRESSED) {*/
            /*numShots++;*/
        /*}*/
        /*shooterLimitPressed = digitalRead(SHOOTER_LIMIT);*/
        /*delay(20);*/
    /*}*/
    /*shoot(0);*/
}

