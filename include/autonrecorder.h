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
#define AUTON_BUTTON 5

/**
 * Lower limit of the autonomous routine selector potentiometer.
 */
#define AUTON_POT_LOW 0

/**
 * Upper limit of the autonomous routine selector potentiometer.
 */
#define AUTON_POT_HIGH 4095

/**
 * Structure containing the joystick state variables at a point in time.
 */
typedef struct joyState {
    signed char spd, turn, sht, intk, trans;
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
 */
extern int progSkills;

/** 
 * Initializes autonomous recorder by setting states array to zero.
 */
void initAutonRecorder();

/** 
 * Records driver joystick values into states array.
 */
void recordAuton();

/** 
 * Saves contents of the states array to a file in flash memory.
 */
void saveAuton();

/** 
 * Loads autonomous file contents into states array.
 */
void loadAuton();

/** 
 * Replays autonomous based on loaded values in states array.
 */
void playbackAuton();

#endif
