#ifndef AUTONRECORDER_H
#define AUTONRECORDER_H

#define AUTON_TIME 15
#define PROGSKILL_TIME 60

#define JOY_POLL_FREQ 50

#define MAX_AUTON_SLOTS 10
#define AUTON_FILENAME_MAX_LENGTH 8

#define AUTON_POT 1
#define AUTON_BUTTON 5
#define AUTON_POT_LOW 0
#define AUTON_POT_HIGH 4095

typedef struct joyState {
    signed char spd, turn, sht, intk, trans;
} joyState;

extern joyState states[AUTON_TIME*JOY_POLL_FREQ];

extern int autonLoaded;
extern int progSkills;

void initAutonRecorder();
void recordAuton();
void saveAuton();
void loadAuton();
void playbackAuton();

#endif
