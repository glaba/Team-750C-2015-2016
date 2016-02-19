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
    lcdSetText(LCD_PORT, 1, "Hardcoded Skills");
    /*while (numShots < 32) {*/
        /*if (shooterLimitPressed == PRESSED && digitalRead(SHOOTER_LIMIT) == UNPRESSED) {*/
            /*numShots++;*/
        /*}*/
        /*shooterLimitPressed = digitalRead(SHOOTER_LIMIT);*/
        /*lcdPrint(LCD_PORT, 2, "Shot: %d", numShots);*/
        /*if (joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {*/
            /*printf("Skills manually cancelled.\n");*/
            /*lcdSetText(LCD_PORT, 1, "Cancelled skills.");*/
            /*lcdSetText(LCD_PORT, 2, "");*/
            /*motorStopAll();*/
            /*return;*/
        /*}*/
        /*delay(20);*/
    /*}*/
    shoot(0);
    gyroReset(gyro);
    int prev_ang = gyroGet(gyro);
    bool done = false;
    while (!done) { //turn right
        printf("Curr: %d\tPrev: %d\tTarg: %d\n", (gyroGet(gyro) % ROTATION_DEG), prev_ang, (-90-CLOSE_GOAL_ANGLE));
        move(0, targetNet(-90-CLOSE_GOAL_ANGLE));
        printf("Turn: %d\n", constrain(turn, -127, 127));
        printf("----------------------------------\n");
        lcdPrint(LCD_PORT, 2, "Angle: %d", (gyroGet(gyro) % ROTATION_DEG));
        if (joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        if(abs((gyroGet(gyro) % ROTATION_DEG) - (-90-CLOSE_GOAL_ANGLE))<=1 && prev_ang == gyroGet(gyro)){
            done = true;
        }
        prev_ang = gyroGet(gyro);
        delay(20);
    }
    move(0, 0);
    gyroReset(gyro);
    /*while(true){*/
        /*printf("Dist: %d\n", ultrasonicGet(sonar));*/
        /*lcdPrint(LCD_PORT, 2, "Dist: %d", ultrasonicGet(sonar));*/
        /*if (joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {*/
            /*printf("Skills manually cancelled.\n");*/
            /*lcdSetText(LCD_PORT, 1, "Cancelled skills.");*/
            /*lcdSetText(LCD_PORT, 2, "");*/
            /*motorStopAll();*/
            /*return;*/
        /*}*/
        /*delay(20);*/
    /*}*/
    while (ultrasonicGet(sonar) > (DISTANCE_TO_OTHER_SIDE + 30) || ultrasonicGet(sonar) == 0) {
        move(127, 0);
        if (joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        delay(20);
    }
    while (ultrasonicGet(sonar) > DISTANCE_TO_OTHER_SIDE) {
        move(64, 0);
        if (joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        delay(20);
    }
    move(0, 0);
    /*integral = 0;*/
    /*prev_error = 0;*/
    /*while (gyroGet(gyro) % ROTATION_DEG != 90 [> turning left <]) {*/
        /*float error = 90 - gyroGet(gyro) % ROTATION_DEG;*/
        /*integral += error * 20;*/
        /*derivative = (error-prev_error)/20;*/
        /*move(0, error * GYRO_KP + integral * GYRO_KI + derivative * GYRO_KD);*/
        /*prev_error = error;*/
        /*lcdPrint(LCD_PORT, 2, "Angle: %d", gyroGet(gyro));*/
        /*if (joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {*/
            /*printf("Skills manually cancelled.\n");*/
            /*lcdSetText(LCD_PORT, 1, "Cancelled skills.");*/
            /*lcdSetText(LCD_PORT, 2, "");*/
            /*motorStopAll();*/
            /*return;*/
        /*}*/
        /*delay(20);*/
    /*}*/
    move(0, 0);

    /*numShots = 0;*/
    shoot(127);
    delay(20);
    shoot(0);
    /*shooterLimitPressed = UNPRESSED;*/
    taskCreate(playSpeaker, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
    //shoot(-127);
    lcdSetText(LCD_PORT, 2, "Final Shots");
    while (true) {
        if (joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {
            printf("Skills manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled skills.");
            lcdSetText(LCD_PORT, 2, "");
            motorStopAll();
            return;
        }
        delay(20);
    }
    /*while (numShots <= 32) {*/
        /*if (shooterLimitPressed == PRESSED && digitalRead(SHOOTER_LIMIT) == UNPRESSED) {*/
            /*numShots++;*/
        /*}*/
        /*shooterLimitPressed = digitalRead(SHOOTER_LIMIT);*/
        /*delay(20);*/
    /*}*/
    /*shoot(0);*/
}

