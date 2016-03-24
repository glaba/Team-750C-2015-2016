/** @file autonroutines.c
 * @brief File for hard-coded autonomous routines
 *
 * This file contains code for hard-coded autonomous routines that use sensors.
 */

#include "main.h"

/**
 * Runs a programming skills routine using sensors rather than the autonomous recorder.
 * Starts in the left side of the field shooting into the closer goal.
 * After shooting into the left net, it turns and goes to the other side of the field.
 * It turns to the face the other net to shoot the final set of match loads.
 */
void runHardCodedProgrammingSkills() {
    lcdSetText(LCD_PORT, 1, "Hardcoded Skills");
    int numShots = 0;
    bool shooterLimitPressed = UNPRESSED;
    shoot(-127);
    while (numShots < 32) {
        if (shooterLimitPressed == PRESSED && digitalRead(SHOOTER_LIMIT) == UNPRESSED) {
            numShots++;
        }
        shooterLimitPressed = digitalRead(SHOOTER_LIMIT);
        lcdPrint(LCD_PORT, 2, "Shot: %d", numShots);
        if (joystickGetDigital(1, 7, JOY_UP)) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        delay(20);
    }
    shoot(0);
    resetGyroVariables();
    int prev_ang = gyroGet(gyro);
    bool done = false;
    int timeout = 0;
    while (!done) { //turn right
        printf("Curr: %d\tPrev: %d\tTarg: %d\n", (gyroGet(gyro) % ROTATION_DEG), prev_ang, (-90-CLOSE_GOAL_ANGLE));
        move(0, targetNet(-90-CLOSE_GOAL_ANGLE), 0);
        printf("Turn: %d\n", constrain(turn, -127, 127));
        printf("----------------------------------\n");
        lcdPrint(LCD_PORT, 2, "Angle: %d", (gyroGet(gyro) % ROTATION_DEG));
        if (joystickGetDigital(1, 7, JOY_UP)) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        if(abs((gyroGet(gyro) % ROTATION_DEG) - (-90-CLOSE_GOAL_ANGLE))<=1 && prev_ang == gyroGet(gyro)) {
            done = true;
        } else if(prev_ang == gyroGet(gyro)) {
            timeout += 20;
        } else {
            timeout = 0;
        }
        prev_ang = gyroGet(gyro);
        delay(20);
        if(timeout >= 1000) {
            done = true;
        }
    }
    move(0, 0, 0);
    delay(200);
    resetGyroVariables();
    int forwspd = 0;
    resetEncoderVariables();
    while (ultrasonicGet(sonar) > (DISTANCE_TO_OTHER_SIDE + 50) || ultrasonicGet(sonar) == 0) {
        moveStraight(constrain(forwspd, -127, 127));
        printf("Fast Dist: %d\n", ultrasonicGet(sonar));
        if (joystickGetDigital(1, 7, JOY_UP)) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        forwspd += 5;
        delay(20);
    }
    while (ultrasonicGet(sonar) > DISTANCE_TO_OTHER_SIDE || ultrasonicGet(sonar) == 0) {
        moveStraight(constrain(forwspd, 64, 127));
        printf("Slow Dist: %d\n", ultrasonicGet(sonar));
        if (joystickGetDigital(1, 7, JOY_UP)) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        forwspd -= 20;
        delay(20);
    }
    move(0, 0, 0);
    delay(200);
    resetGyroVariables();
    prev_ang = gyroGet(gyro);
    done = false;
    timeout = 0;
    while (!done) { //turn left
        printf("Curr: %d\tPrev: %d\tTarg: %d\n", (gyroGet(gyro) % ROTATION_DEG), prev_ang, 90+FAR_GOAL_ANGLE);
        move(0, targetNet(90+FAR_GOAL_ANGLE), 0);
        printf("Turn: %d\n", constrain(turn, -127, 127));
        printf("----------------------------------\n");
        lcdPrint(LCD_PORT, 2, "Angle: %d", (gyroGet(gyro) % ROTATION_DEG));
        if (joystickGetDigital(1, 7, JOY_UP)) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        if(abs((gyroGet(gyro) % ROTATION_DEG) - (90+FAR_GOAL_ANGLE))<=1 && prev_ang == gyroGet(gyro)) {
            done = true;
        } else if(prev_ang == gyroGet(gyro)) {
            timeout += 20;
        } else {
            timeout = 0;
        }
        prev_ang = gyroGet(gyro);
        delay(20);
        if(timeout >= 1000) {
            done = true;
        }
    }
    move(0, 0, 0);
    resetGyroVariables();
    shoot(127);
    delay(20);
    shoot(0);
    taskCreate(playSpeaker, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
    delay(200);
    shoot(-127);
    lcdSetText(LCD_PORT, 2, "Final Shots");
    while (true) {
        if (joystickGetDigital(1, 7, JOY_UP)) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        delay(20);
    }
}

