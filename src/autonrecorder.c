/** @file autonrecorder.c
 * @brief File for autonomous recorder code
 *
 * This file contains the code for the saving, loading, and playback of autonomous files.
 * When an autonomous routine is recorded, it is saved to a file to flash memory.
 * This file is loaded and executed during the autonomous period of the game.
 * It works by saving the motor values at a point in time.
 * At the corresponding point in time, the values are played back.
 *
 * This file also handles the recording of programming skills by stitching 4 autonomous routines together.
 */

#include "main.h"

/**
 * Stores the joystick state variables for moving the robot.
 * Used for recording and playing back autonomous routines.
 */
joyState states[AUTON_TIME*JOY_POLL_FREQ];

/**
 * Slot number of currently loaded autonomous routine.
 */
int autonLoaded;

/**
 * Section number (0-3) of currently loaded programming skills routine.
 */
int progSkills;

/** 
 * Selects which autonomous file to use based on the potentiometer reading.
 * 
 * @return the autonomous selected (slot number)
 */
int selectAuton() {
    bool done = false;
    int val;
    do {
        val = (float) ((float) analogRead(AUTON_POT)/(float) AUTON_POT_HIGH) * (MAX_AUTON_SLOTS+3);
        if(val > MAX_AUTON_SLOTS+2){
            val = MAX_AUTON_SLOTS+2;
        }
        if(val == 0) {
            lcdSetText(LCD_PORT, 2, "NONE");
        } else if(val == MAX_AUTON_SLOTS+1) {
            lcdSetText(LCD_PORT, 2, "Prog. Skills");
        } else if (val == MAX_AUTON_SLOTS+2) {
            lcdSetText(LCD_PORT, 2, "Hardcoded Skills");
        } else {
            char filename[AUTON_FILENAME_MAX_LENGTH];
            snprintf(filename, sizeof(filename)/sizeof(char), "a%d", val);
            FILE* autonFile = fopen(filename, "r");
            if(autonFile == NULL){
                lcdPrint(LCD_PORT, 2, "Slot: %d (EMPTY)", val);
            } else {
                char name[LCD_MESSAGE_MAX_LENGTH+1];
                memset(name, 0, sizeof(name));
                fread(name, sizeof(char), sizeof(name) / sizeof(char), autonFile);
                lcdSetText(LCD_PORT, 2, name);
                fclose(autonFile);
            }
        }
        done = (digitalRead(AUTON_BUTTON) == PRESSED);
        delay(20);
    } while(!done);
    printf("Selected autonomous: %d\n", val);
    return val;
}

/** 
 * Initializes autonomous recorder by setting states array to zero.
 */
void initAutonRecorder() {
    printf("Beginning initialization of autonomous recorder...\n");
    lcdClear(LCD_PORT);
    lcdSetText(LCD_PORT, 1, "Init recorder...");
    lcdSetText(LCD_PORT, 2, "");
    memset(states, 0, sizeof(*states));
    printf("Completed initialization of autonomous recorder.\n");
    lcdSetText(LCD_PORT, 1, "Init-ed recorder!");
    lcdSetText(LCD_PORT, 2, "");
    autonLoaded = -1;
    progSkills = 0;
}

/** 
 * Records driver joystick values into states array.
 */
void recordAuton() {
    lcdClear(LCD_PORT);
    for(int i = 3; i > 0; i--){
        lcdSetBacklight(LCD_PORT, true);
        printf("Beginning autonomous recording in %d...\n", i);
        lcdSetText(LCD_PORT, 1, "Recording auton");
        lcdPrint(LCD_PORT, 2, "in %d...", i);
        delay(1000);
    }
    printf("Ready to begin autonomous recording.\n");
    lcdSetText(LCD_PORT, 1, "Recording auton...");
    lcdSetText(LCD_PORT, 2, "");
    bool lightState = false;
    for (int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
        printf("Recording state %d...\n", i);
        lcdSetBacklight(LCD_PORT, lightState);
        lightState = !lightState;
        recordJoyInfo();
        states[i].spd = spd;
        states[i].turn = turn;
        states[i].sht = sht;
        states[i].intk = intk;
        states[i].strafe = strafe;
        states[i].ang = ang;
        states[i].liftL = liftL;
        states[i].liftR = liftR;
        if (joystickGetDigital(1, 7, JOY_UP)) {
            printf("Autonomous recording manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled record.");
            lcdSetText(LCD_PORT, 2, "");
            memset(states + i + 1, 0, sizeof(joyState) * (AUTON_TIME * JOY_POLL_FREQ - i - 1));
            i = AUTON_TIME * JOY_POLL_FREQ;
        }
        moveRobot();
        delay(1000 / JOY_POLL_FREQ);
    }
    printf("Completed autonomous recording.\n");
    lcdSetText(LCD_PORT, 1, "Recorded auton!");
    lcdSetText(LCD_PORT, 2, "");
    motorStopAll();
    delay(1000);
    autonLoaded = 0;
}

/** 
 * Saves contents of the states array to a file in flash memory.
 */
void saveAuton() {
    printf("Waiting for file selection...\n");
    lcdClear(LCD_PORT);
    lcdSetText(LCD_PORT, 1, "Save to?");
    lcdSetText(LCD_PORT, 2, "");
    int autonSlot;
    if(progSkills == 0) {
        autonSlot = selectAuton();
    } else {
        printf("Currently in the middle of a programming skills run.\n");
        autonSlot = MAX_AUTON_SLOTS + 1;
    }
    char name[LCD_MESSAGE_MAX_LENGTH+1];
    memset(name, 0, sizeof(name));
    if(autonSlot == 0) {
        printf("Not saving this autonomous!\n");
        return;
    } else if(autonSlot != MAX_AUTON_SLOTS+1) {
        typeString(name);
    }
    lcdSetText(LCD_PORT, 1, "Saving auton...");
    char filename[AUTON_FILENAME_MAX_LENGTH];
    if(autonSlot != MAX_AUTON_SLOTS + 1) {
        printf("Not doing programming skills, recording to slot %d.\n",autonSlot);
        snprintf(filename, sizeof(filename)/sizeof(char), "a%d", autonSlot);
        //lcdPrint(LCD_PORT, 2, "Slot: %d", autonSlot);
        lcdPrint(LCD_PORT, 2, "%s", name);
    } else {
        printf("Doing programming skills, recording to section %d.\n", progSkills);
        snprintf(filename, sizeof(filename)/sizeof(char), "p%d", progSkills);
        lcdPrint(LCD_PORT, 2, "Skills Part: %d", progSkills+1);
    }
    printf("Saving to file %s...\n",filename);
    FILE *autonFile = fopen(filename, "w");
    if (autonFile == NULL) {
        printf("Error saving autonomous in file %s!\n", filename);
        lcdSetText(LCD_PORT, 1, "Error saving!");
        if(autonSlot != MAX_AUTON_SLOTS + 1){
            printf("Not doing programming skills, error saving auton in slot %d!\n", autonSlot);
            lcdSetText(LCD_PORT, 1, "Error saving!");
            lcdPrint(LCD_PORT, 2,   "Slot: %d", autonSlot);
        } else {
            printf("Doing programming skills, error saving auton in section 0!\n");
            lcdSetText(LCD_PORT, 1, "Error saving!");
            lcdSetText(LCD_PORT, 2, "Prog. Skills");
        }
        delay(1000);
        return;
    }
    if(autonSlot != MAX_AUTON_SLOTS+1){
        fwrite(name, sizeof(char), sizeof(name) / sizeof(char), autonFile);
    }
    for (int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
        printf("Recording state %d to file %s...\n", i, filename);
        signed char write[8] = {states[i].spd, states[i].turn, states[i].sht, states[i].intk, states[i].strafe,
                                states[i].ang, states[i].liftL, states[i].liftR};
        fwrite(write, sizeof(char), sizeof(write) / sizeof(char), autonFile);
        delay(10);
    }
    fclose(autonFile);
    printf("Completed saving autonomous to file %s.\n", filename);
    lcdSetText(LCD_PORT, 1, "Saved auton!");
    if(autonSlot != MAX_AUTON_SLOTS + 1) {
        printf("Not doing programming skills, recorded to slot %d.\n",autonSlot);
        lcdPrint(LCD_PORT, 2, "Slot: %d", autonSlot);
    } else {
        printf("Doing programming skills, recorded to section %d.\n", progSkills);
        lcdPrint(LCD_PORT, 2, "Skills Part: %d", progSkills+1);
    }
    delay(1000);
    if(autonSlot == MAX_AUTON_SLOTS + 1) {
        printf("Proceeding to next programming skills section (%d).\n", ++progSkills);
    }
    if(progSkills == PROGSKILL_TIME/AUTON_TIME) {
        printf("Finished recording programming skills (all parts).\n");
        progSkills = 0;
    }
    autonLoaded = autonSlot;
}

/** 
 * Loads autonomous file contents into states array.
 */
void loadAuton() {
    lcdClear(LCD_PORT);
    bool done = false;
    int autonSlot;
    FILE* autonFile;
    char filename[AUTON_FILENAME_MAX_LENGTH];
    do {
        printf("Waiting for file selection...\n");
        lcdSetText(LCD_PORT, 1, "Load from?");
        lcdSetText(LCD_PORT, 2, "");
        autonSlot = selectAuton();
        if(autonSlot == 0) {
            printf("Not loading an autonomous!\n");
            lcdSetText(LCD_PORT, 1, "Not loading!");
            lcdSetText(LCD_PORT,   2, "");
            autonLoaded = 0;
            return;
        } else if(autonSlot == MAX_AUTON_SLOTS + 1){
            printf("Performing programming skills.\n");
            lcdSetText(LCD_PORT, 1, "Loading skills...");
            lcdPrint(LCD_PORT,   2, "Skills Part: 1");
            autonLoaded = MAX_AUTON_SLOTS + 1;
        } else if (autonSlot == MAX_AUTON_SLOTS + 2) {
            printf("Performing hard-coded programming skills.\n");
            lcdSetText(LCD_PORT, 1, "Loaded skills!");
            lcdPrint(LCD_PORT,   2, "Hardcoded Skills");
            autonLoaded = MAX_AUTON_SLOTS + 2;
            return;
        } else if(autonSlot == autonLoaded) {
            printf("Autonomous %d is already loaded.\n", autonSlot);
            lcdSetText(LCD_PORT, 1, "Loaded auton!");
            lcdPrint(LCD_PORT,   2, "Slot: %d", autonSlot);
            return;
        }
        printf("Loading autonomous from slot %d...\n", autonSlot);
        lcdSetText(LCD_PORT, 1, "Loading auton...");
        if(autonSlot != MAX_AUTON_SLOTS + 1){
            lcdPrint(LCD_PORT, 2,   "Slot: %d", autonSlot);
        }
        if(autonSlot != MAX_AUTON_SLOTS + 1){
            printf("Not doing programming skills, loading slot %d\n", autonSlot);
            snprintf(filename, sizeof(filename)/sizeof(char), "a%d", autonSlot);
        } else {
            printf("Doing programming skills, loading section 0.\n");
            snprintf(filename, sizeof(filename)/sizeof(char), "p0");
        }
        printf("Loading from file %s...\n",filename);
        autonFile = fopen(filename, "r");
        if (autonFile == NULL) {
            printf("No autonomous was saved in file %s!\n", filename);
            lcdSetText(LCD_PORT, 1, "No auton saved!");
            if(autonSlot != MAX_AUTON_SLOTS + 1){
                printf("Not doing programming skills, no auton in slot %d!\n", autonSlot);
                lcdSetText(LCD_PORT, 1, "No auton saved!");
                lcdPrint(LCD_PORT, 2,   "Slot: %d", autonSlot);
            } else {
                printf("Doing programming skills, no auton in section 0!\n");
                lcdSetText(LCD_PORT, 1, "No skills saved!");
            }
            delay(1000);
        } else {
            done = true;
        }
    } while(!done);
    fseek(autonFile, 0, SEEK_SET);
    char name[LCD_MESSAGE_MAX_LENGTH+1];
    memset(name, 0, sizeof(name));
    if(autonSlot != MAX_AUTON_SLOTS + 1){
        fread(name, sizeof(char), sizeof(name) / sizeof(char), autonFile);
    }
    for (int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
        printf("Loading state %d from file %s...\n", i, filename);
        char read[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        fread(read, sizeof(char), sizeof(read) / sizeof(char), autonFile);
        states[i].spd = (signed char) read[0];
        states[i].turn = (signed char) read[1];
        states[i].sht = (signed char) read[2];
        states[i].intk = (signed char) read[3];
        states[i].strafe = (signed char) read[4];
        states[i].ang = (signed char) read[5];
        states[i].liftL = (signed char) read[6];
        states[i].liftR = (signed char) read[7];
        delay(10);
    }
    fclose(autonFile);
    printf("Completed loading autonomous from file %s.\n", filename);
    lcdSetText(LCD_PORT, 1, "Loaded auton!");
    if(autonSlot != MAX_AUTON_SLOTS + 1){
        printf("Not doing programming skills, loaded from slot %d.\n", autonSlot);
        //lcdPrint(LCD_PORT,   2, "Slot: %d", autonSlot);
        lcdPrint(LCD_PORT, 2, "%s", name);
    } else {
        printf("Doing programming skills, loaded from section %d.\n", progSkills);
        lcdSetText(LCD_PORT, 2, "Skills Section: 1");
    }
    autonLoaded = autonSlot;
}

/** 
 * Replays autonomous based on loaded values in states array.
 */
void playbackAuton() { //must load autonomous first!
    if (autonLoaded == -1 /* nothing in memory */) {
        printf("No autonomous loaded, entering loadAuton()\n");
        loadAuton();
    }
    if(autonLoaded == 0) {
        printf("autonLoaded = 0, doing nothing.\n");
        return;
    } else if (autonLoaded == MAX_AUTON_SLOTS + 2) {
        runHardCodedProgrammingSkills();
        return;
    }
    printf("Beginning playback...\n");
    lcdSetText(LCD_PORT, 1, "Playing back...");
    lcdSetText(LCD_PORT, 2, "");
    lcdSetBacklight(LCD_PORT, true);
    int file=0;
    do{
        FILE* nextFile = NULL;
        lcdPrint(LCD_PORT, 2, "File: %d", file+1);
        char filename[AUTON_FILENAME_MAX_LENGTH];
        if(autonLoaded == MAX_AUTON_SLOTS + 1 && file < PROGSKILL_TIME/AUTON_TIME - 1){
            printf("Next section: %d\n", file+1);
            snprintf(filename, sizeof(filename)/sizeof(char), "p%d", file+1);
            nextFile = fopen(filename, "r");
        }
        for(int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
            printf("Playing back state %d...\n", i);
            spd = states[i].spd;
            turn = states[i].turn;
            sht = states[i].sht;
            intk = states[i].intk;
            strafe = states[i].strafe;
            ang = states[i].ang;
            liftL = states[i].liftL;
            liftR = states[i].liftR;
            if (joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {
                printf("Playback manually cancelled.\n");
                lcdSetText(LCD_PORT, 1, "Cancelled playback.");
                lcdSetText(LCD_PORT, 2, "");
                i = AUTON_TIME * JOY_POLL_FREQ;
                file = PROGSKILL_TIME/AUTON_TIME;
            }
            moveRobot();
            if(autonLoaded == MAX_AUTON_SLOTS + 1 && file < PROGSKILL_TIME/AUTON_TIME - 1){
                printf("Loading state %d from file %s...\n", i, filename);
                char read[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                fread(read, sizeof(char), sizeof(read) / sizeof(char), nextFile);
                states[i].spd = (signed char) read[0];
                states[i].turn = (signed char) read[1];
                states[i].sht = (signed char) read[2];
                states[i].intk = (signed char) read[3];
                states[i].strafe = (signed char) read[4];
                states[i].ang = (signed char) read[5];
                states[i].liftL = (signed char) read[6];
                states[i].liftR = (signed char) read[7];
            }
            delay(1000 / JOY_POLL_FREQ);
        }
        if(autonLoaded == MAX_AUTON_SLOTS + 1 && file < PROGSKILL_TIME/AUTON_TIME - 1){
            printf("Finished with section %d, closing file.\n", file+1);
            fclose(nextFile);
        }
        file++;
    } while(autonLoaded == MAX_AUTON_SLOTS + 1 && file < PROGSKILL_TIME/AUTON_TIME);
    motorStopAll();
    printf("Completed playback.\n");
    lcdSetText(LCD_PORT, 1, "Played back!");
    lcdSetText(LCD_PORT, 2, "");
    delay(1000);
}

