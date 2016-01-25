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
    bool shooterLimitPressed = PRESSED;
    shoot(-127);
    while (numShots - 1 <= 32 /* one shot subtracted because the shooter starts uncocked */) {
        if (shooterLimitPressed == PRESSED && digitalRead(SHOOTER_LIMIT) == UNPRESSED) {
            numShots++;
        }
        shooterLimitPressed = digitalRead(SHOOTER_LIMIT);
        delay(20);
    }
    shoot(0);

    while (gyroGet(gyro) % ROTATION_DEG != -90 - CLOSE_GOAL_ANGLE /* turning right */) {
        int error = (-90 - CLOSE_GOAL_ANGLE) - gyroGet(gyro) % ROTATION_DEG;
        move(0, error * GYRO_P);
    }
    while (ultrasonicGet(sonar) < DISTANCE_TO_OTHER_SIDE - 30) {
        move(127, 0);
    }
    while (ultrasonicGet(sonar) < DISTANCE_TO_OTHER_SIDE) {
        move(64, 0);
    }
    while (gyroGet(gyro) % ROTATION_DEG != -2*CLOSE_GOAL_ANGLE /* turning left */) {
        int error = (-2*CLOSE_GOAL_ANGLE) - gyroGet(gyro) % ROTATION_DEG;
        move(0, error * GYRO_P);
    }

    numShots = 0;
    shooterLimitPressed = digitalRead(SHOOTER_LIMIT);
    shoot(-127);
    while (numShots - 1 <= 32 /* one shot subtracted because the shooter starts uncocked */) {
        if (shooterLimitPressed == PRESSED && digitalRead(SHOOTER_LIMIT) == UNPRESSED) {
            numShots++;
        }
        shooterLimitPressed = digitalRead(SHOOTER_LIMIT);
        delay(20);
    }
    shoot(0);
}

