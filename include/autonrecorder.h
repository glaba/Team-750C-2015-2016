/** @file autonrecorder.h
 * @brief Header file for autonomous recorder functions and definitions
 *
 * This file contains definitions and function declarations for the autonomous recorder.
 * These definitions provide fundamental constants and classes that the autonomous recorder uses.
 * Additionally, this file defines the autonomous selection potentiometer and button.
 * It also provides access to the autonomous recorder functions from other files.
 * This allows for the recorder to be accessed during operator control.
 */

#ifndef AUTONRECORDER_H
#define AUTONRECORDER_H

/**
 * Number of seconds the autonomous period lasts.
 */
#define AUTON_TIME 15

/**
 * Number of seconds the programming skills challenge lasts.
 */
#define PROGSKILL_TIME 60

/**
 * Frequency to poll the joystick for recording.
 * The joystick values will be recorded this many times per second.
 * The joystick updates every 20 milliseconds (50 times per second).
 */
#define JOY_POLL_FREQ 50

/**
 * Maximum number of autonomous routines to be stored.
 */
#define MAX_AUTON_SLOTS 10

/**
 * Maximum file name length of autonomous routine files.
 */
#define AUTON_FILENAME_MAX_LENGTH 8

/**
 * Potentiometer for selecting which autonomous routine to load.
 */
#define AUTON_POT 1

/**
 * Button for confirming selection of an autonomous routine.
 */
#define AUTON_BUTTON 9

/**
 * Lower limit of the autonomous routine selector potentiometer.
 */
#define AUTON_POT_LOW 0

/**
 * Upper limit of the autonomous routine selector potentiometer.
 */
#define AUTON_POT_HIGH 440 //4095

/**
 * @brief Representation of the operator controller's instructions at a point in time.
 * 
 * This state represents the values of the motors at a point in time.
 * These instructions are played back at the rate polled to send the same commands the operator did.
 */
typedef struct joyState {
    /**
    * Forward/backward speed of the drive motors.
    */
    signed char spd;

    /**
    * Turning speed of the drive motors.
    */
    signed char turn;

    /**
    * Speed of the shooter motors.
    */
    signed char sht; 

    /**
    * Speed of the intake motor.
    */
    signed char intk;

    /**
    * Speed of the strafing motor.
    */
    signed char strafe;

    /**
    * Speed of the angle change motor.
    */
    signed char ang;

    /**
     * Speed of the left lift motor.
     */
    signed char liftL;

    /**
     * Speed of the right lift motor.
     */
    signed char liftR;
} joyState;

/**
 * Stores the joystick state variables for moving the robot.
 * Used for recording and playing back autonomous routines.
 */
extern joyState states[AUTON_TIME*JOY_POLL_FREQ];

/**
 * Slot number of currently loaded autonomous routine.
 */
extern int autonLoaded;

/**
 * Section number (0-3) of currently loaded programming skills routine.
 * Since programming skills lasts for 60 seconds, it can be represented by 4 standard autonomous recordings.
 */
extern int progSkills;

/** 
 * Initializes autonomous recorder by setting joystick states array to zero.
 */
void initAutonRecorder();

/** 
 * Records driver joystick values into states array for saving.
 */
void recordAuton();

/** 
 * Saves contents of the states array to a file in flash memory for later playback.
 */
void saveAuton();

/** 
 * Loads autonomous file contents into states array for playback.
 */
void loadAuton();

/** 
 * Replays autonomous based on loaded values in states array.
 */
void playbackAuton();

#endif

