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

int spd;
int turn;
int sht; 
int intk;
int trans;

void recordJoyInfo(){
    spd = joystickGetAnalog(1, 3);
    turn = joystickGetAnalog(1, 1);
    sht = 0;
    intk = 0;
    trans = 0;
    if(joystickGetDigital(1, 6, JOY_UP)){
        sht = 127;
    } else if(joystickGetDigital(1, 6, JOY_DOWN)){
        sht = -127;
    } else {
        sht = 0;
    }
    if(joystickGetDigital(1, 5, JOY_UP)){
        intk = 127;
    } else if(joystickGetDigital(1, 5, JOY_DOWN)){
        intk = -127;
    } else {
        intk = 0;
    }
    if(joystickGetDigital(1, 8, JOY_LEFT)){
        trans = 40;
        /*changeGear(GEAR_LIFT);*/
    } else if(joystickGetDigital(1, 8, JOY_RIGHT)){
        trans = -40;
        /*changeGear(GEAR_DRIVE);*/
    } else {
        trans = 0;
    }
}

void moveRobot(){
    move(spd, turn);
    shoot(sht);
    intake(intk);
    transmission(trans);
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
    lcdSetBacklight(LCD_PORT, false);
    while (true) {
        if(isOnline() || progSkills == 0){
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
                    saveAuton();
                } else if (joystickGetDigital(1, 7, JOY_LEFT) && !isOnline()) {
                    taskSuspend(lcdDiagTask);
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
