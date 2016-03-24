/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Copyright (c) 2011-2014, Purdue University ACM SIG BOTS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Purdue University ACM SIG BOTS nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PURDUE UNIVERSITY ACM SIG BOTS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Purdue Robotics OS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/**
 * Forward/backward speed of the drive motors.
 */
int spd;

/**
 * Turning speed of the drive motors.
 */
int turn;

/**
 * Speed of the shooter motors.
 */
int sht; 

/**
 * Speed of the intake motors.
 */
int intk;

/**
 * Speed of the strafing motor.
 */
int strafe;

/**
 * Speed of the lift deployment motor.
 */
int dep;

float previous_error = 0;
float integral = 0;
float derivative = 0;

/** 
 * Faces the robot towards the desired gyroscope angle by turning it.
 * This function implements a simple PID control loop in order to correct for error.
 * 
 * @param target the desired robot angle
 * @return the drive turning speed value
 */
int targetNet(int target){
    float error = -1 * (target - (gyroGet(gyro) % ROTATION_DEG));
    integral += error * 20;
    derivative = (error-previous_error)/100.0;
    printf("P: %f\tI: %f\tD: %f\n", error, integral, derivative);
    turn = error * GYRO_KP + integral * GYRO_KI + derivative * GYRO_KD;
    previous_error = error;
    return turn;
}

/** 
 * Resets the gyroscope PID variables so it can be used a second time.
 */
void resetGyroVariables(){
    gyroReset(gyro);
    integral = 0;
    derivative = 0;
    previous_error = 0;
}

/**
 * Populates the motor state variables based on the joystick's current values.
 */
void recordJoyInfo(){
    spd = joystickGetAnalog(1, 3);
    turn = joystickGetAnalog(1, 1);
    strafe = joystickGetAnalog(1, 4);
    sht = 0;
    intk = 0;
    if(abs(joystickGetAnalog(2, 3))<30 && abs(joystickGetAnalog(2, 2))<30){
        if(joystickGetDigital(1, 6, JOY_UP) || joystickGetDigital(2, 6, JOY_UP)){
            sht = 127;
        } else if(joystickGetDigital(1, 6, JOY_DOWN) || joystickGetDigital(2, 6, JOY_DOWN)){
            sht = -127;
        } else if(joystickGetDigital(2, 7, JOY_LEFT)){
            sht = 70;
        } else if(joystickGetDigital(2, 7, JOY_DOWN)){
            sht = -60;
        } else if(joystickGetDigital(2, 7, JOY_RIGHT)){
            sht = -50;
        } else {
            sht = 0;
        }
    } else if(abs(joystickGetAnalog(2, 3))<30){
        sht = joystickGetAnalog(2, 2);
    } else {
        sht = joystickGetAnalog(2, 3);
    }
    if(joystickGetDigital(1, 5, JOY_UP) || joystickGetDigital(2, 5, JOY_UP)){
        intk = 127;
    } else if(joystickGetDigital(1, 5, JOY_DOWN) || joystickGetDigital(2, 5, JOY_DOWN)){
        intk = -127;
    } else {
        intk = 0;
    }
    if(joystickGetDigital(1, 8, JOY_UP) || joystickGetDigital(2, 8, JOY_UP)){
        dep = 127;
    } else if(joystickGetDigital(1, 8, JOY_DOWN) || joystickGetDigital(2, 8, JOY_DOWN)){
        dep = -127;
    } else {
        dep = 0;
    }

    if(joystickGetDigital(1, 7, JOY_DOWN)){
        targetNet(GYRO_NET_TARGET);
    } else if(joystickGetDigital(1, 7, JOY_UP)){
        resetGyroVariables();
    }
}

/**
 * Moves the robot based on the motor state variables.
 */
void moveRobot(){
    move(spd, turn, strafe);
    shoot(sht);
    intake(intk);
    deploy(dep);
}

/**
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
void operatorControl() {
    lcdSetBacklight(LCD_PORT, true);
    bool speakerPlay = false;
    bool speakerButtonPressed = false;
    speakerTask = NULL;
    if(autonLoaded == MAX_AUTON_SLOTS + 2){
        playbackAuton();
    }
    while (true) {
        if(isOnline() || progSkills == 0){
            if(joystickGetDigital(1, 8, JOY_UP) || joystickGetDigital(1, 8, JOY_DOWN) || joystickGetDigital(2, 8, JOY_UP) || joystickGetDigital(2, 8, JOY_DOWN)) {
                if(!speakerButtonPressed) {
                    speakerPlay = !speakerPlay;
                    speakerButtonPressed = true;
                }
            } else {
                speakerButtonPressed = false;
            }
            if(speakerPlay){
                if(speakerTask == NULL){
                    speakerTask = taskCreate(playSpeaker, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
                    speakerPlay = false;
                }
            } 
            if(lcdDiagTask == NULL){
                lcdDiagTask = taskCreate(formatLCDDisplay, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
            } else if(taskGetState(lcdDiagTask) == TASK_SUSPENDED){
                taskResume(lcdDiagTask);
            }
            if(!disableOpControl){
                recordJoyInfo();
                if (joystickGetDigital(1, 7, JOY_RIGHT) && !isOnline()) {
                    taskSuspend(lcdDiagTask);
                    recordAuton();
                    lcdSetBacklight(LCD_PORT, true);
                    saveAuton();
                } else if (joystickGetDigital(1, 7, JOY_LEFT) && !isOnline()) {
                    taskSuspend(lcdDiagTask);
                    lcdSetBacklight(LCD_PORT, true);
                    loadAuton();
                    playbackAuton();
                }
                moveRobot();
            }
        } else {
            motorStopAll();
            lcdSetText(LCD_PORT, 1, "Press 7R");
            lcdPrint(LCD_PORT, 2,   "Last Skills: %d", progSkills);
            if (joystickGetDigital(1, 7, JOY_RIGHT) && !isOnline()) {
                recordAuton();
                saveAuton();
            } else if(joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {
                progSkills = 0;
            }
        }
        delay(20);
    }
}

