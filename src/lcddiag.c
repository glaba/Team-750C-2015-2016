/** @file lcddiag.c
 * @brief File for LCD diagnostic menu code
 *
 * This file contains the code for the LCD diagnostic menu.
 * The menu provides live debugging and testing functionality.
 * It provides the following functions:
 *     - Motor testing functionality (individual and group)
 *     - Motor group management
 *     - Battery voltage information
 *     - Joystick connection status
 *     - Robot sensory data
 *     - Autonomous recorder status
 *     - LCD backlight toggle
 *     - Screensaver that displays during operator control
 *     - Credits menu
 *
 * The idea behind this was inspired by Team 750W and Akram Sandhu.
 * Without them, this project would not be possible.
 *
 * Note: the implementation of this feature is completely different between the two teams.
 * No code was reused from their implementation of the LCD diagnostic menu.
 */

#include "main.h"

/**
 * Object representing the LCD diagnostic menu task.
 * The LCD diagnostic menu runs in a separate thread from the operator control code.
 * The TaskHandle allows for pausing and resuming of the LCD diagnostic menu during autonomous recording.
 */
TaskHandle lcdDiagTask = NULL;

/**
 * Boolean representing the LCD screen's backlight state.
 */
bool backlight = true;

/**
 * Disables operator control loop during motor testing.
 * Since running motors is not thread safe, it is necessary to stop operator control of the motors during testing.
 */
bool disableOpControl = false;

/**
 * Array that stores the motor groups.
 * As this is a dynamic array, creating and editing new motor groups is possible.
 * These motor groups are added to the array via the Motor Group Management menu.
 */
MotorGroup *groups;

/**
 * Stores the number of motor groups.
 * This is functionally identical to the size of the motor group array.
 */
int numgroups;

/**
 * Stores the top-level menu names.
 */
char menuChoices[LCD_MENU_COUNT][LCD_MESSAGE_MAX_LENGTH+1] = {
    "Motor Test",
    "Motor Group Mgmt",
    "Battery Info",
    "Connection Info",
    "Robot Info",
    "Autonomous Info",
    "Toggle Backlight",
    "Screensaver",
    "Credits"
};

/**
 * Uses the LCD and the autonomous potentiometer to type a string.
 * This is used to name motor groups and autonomous recordings.
 * The maximum length of string this function can type is 16 characters.
 *
 * @param dest a buffer to store the typed string (must be at least 17 characters to hold null terminator)
 *
 * @return a pointer to the buffer
 */
char* typeString(char *dest){
    bool done = false;
    int val;
    memset(dest, 0, sizeof(*dest));
    int i = 0;
    typedef enum Case {
        UPPER, LOWER, NUMBER
    } Case;
    Case c = UPPER;
    int mult = 28;
    int spacecode = 26;
    int endcode = 27;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        switch(c){
            case UPPER:
            case LOWER: mult = 28; spacecode = 26; endcode = 27; break;
            case NUMBER: mult = 12; spacecode = 10; endcode = 11; break;
        }
        val = (float) ((float) analogRead(AUTON_POT)/(float) AUTON_POT_HIGH) * mult;
        if(val > endcode){
            val = endcode;
        }
        if(val == spacecode){
            dest[i] = ' ';
        } else if(val == endcode) {
            dest[i] = '~';
        } else {
            switch(c){
                case UPPER: dest[i] = val + 'A'; break;
                case LOWER: dest[i] = val + 'a'; break;
                case NUMBER: dest[i] = val + '0'; break;
            }
        }

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(dest))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, dest);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 1, str);

        if(i > 0){
            if(c == UPPER){
                lcdSetText(LCD_PORT, 2, "DEL    SEL   abc");
            } else if(c == LOWER) {
                lcdSetText(LCD_PORT, 2, "DEL    SEL   123");
            } else { //NUMBER
                lcdSetText(LCD_PORT, 2, "DEL    SEL   ABC");
            }
        } else {
            if(c == UPPER){
                lcdSetText(LCD_PORT, 2, "|      SEL   abc");
            } else if(c == LOWER) {
                lcdSetText(LCD_PORT, 2, "|      SEL   123");
            } else { //NUMBER
                lcdSetText(LCD_PORT, 2, "|      SEL   ABC");
            }
        }

        if((centerPressed) && val != endcode){
            i++;
        } else if(leftPressed && i > 0){
            dest[i] = 0;
            i--;
        } else if(rightPressed) {
            switch(c){
                case UPPER: c = LOWER; break;
                case LOWER: c = NUMBER; break;
                case NUMBER: c = UPPER; break;
            }
        }

        done = ((centerPressed && val == endcode) || i == LCD_MESSAGE_MAX_LENGTH);
        delay(20);
    } while(!done);
    dest[i] = 0;
    return dest;
}

/**
 * Saves motor groups out to a file.
 * This file can be loaded to add custom motor groups into memory.
 */
void saveGroups(){
    FILE* group = fopen("grp", "w");
    taskPrioritySet(NULL, TASK_PRIORITY_HIGHEST-1);
    lcdSetText(LCD_PORT, 1, "Saving groups...");
    lcdSetText(LCD_PORT, 2, "");
    for(int i = 0; i < numgroups; i++){
        fwrite(groups[i].motor, sizeof(bool), sizeof(groups[i].motor) / sizeof(bool), group);
        fwrite("\n", sizeof(char), sizeof("\n") / sizeof(char), group);
        fwrite(groups[i].name, sizeof(char), sizeof(groups[i].name) / sizeof(char), group);
        if(i == numgroups-1){
            fwrite("\t", sizeof(char), sizeof("\t") / sizeof(char), group);
        } else {
            fwrite("\n", sizeof(char), sizeof("\n") / sizeof(char), group);
        }
    }
    taskPrioritySet(NULL, TASK_PRIORITY_DEFAULT);
    lcdSetText(LCD_PORT, 1, "Saved groups!");
    lcdSetText(LCD_PORT, 2, "");
    delay(1000);
}

/**
 * Loads motor groups from a file on the Cortex flash memory.
 * This is used to add custom motor groups for testing purposes.
 */
void loadGroups(){
    FILE* group = fopen("grp", "r");
    taskPrioritySet(NULL, TASK_PRIORITY_HIGHEST-1);
    lcdSetText(LCD_PORT, 1, "Loading groups...");
    lcdSetText(LCD_PORT, 2, "");
    if(groups != NULL){
        free(groups);
    }
    groups = NULL;
    int i = 0;
    bool done = false;
    while(!done){
        groups = (MotorGroup *) realloc(groups, sizeof(MotorGroup)*(i+1));
        fread(groups[i].motor, sizeof(bool), sizeof(groups[i].motor) / sizeof(bool), group);
        fgetc(group);
        fread(groups[i].name, sizeof(char), sizeof(groups[i].name) / sizeof(char), group);
        if(fgetc(group) == '\t'){
            done = true;
        }
        i++;
    }
    numgroups = i;
    taskPrioritySet(NULL, TASK_PRIORITY_DEFAULT);
    lcdSetText(LCD_PORT, 1, "Loaded groups!");
    lcdSetText(LCD_PORT, 2, "");
    delay(1000);
}

/**
 * Initializes the motor groups array to contain the standard set of groups.
 * This includes: Left Drive, Right Drive, Full Drive, Nautilus Shooter, Intake, and Transmission.
 */
void initGroups(){
    FILE* group = fopen("grp", "r");
    if(group == NULL){
        numgroups = 6; //LDRIVE, RDRIVE, DRIVE, SHOOT, INTK, TRANS 
        groups = (MotorGroup*) malloc(sizeof(MotorGroup) * numgroups);
        if(groups == NULL){
            return;
        }
        memset(groups, 0, sizeof(*groups));
        for(int i = 0; i<numgroups; i++){
            for(int j = 0; j<=10; j++){
                groups[i].motor[j] = false;
            }
        }
        groups[0].motor[LEFT_MOTOR_TOP] = true;
        groups[0].motor[LEFT_MOTOR_BOT] = true;
        strcpy(groups[0].name, "Left Drive");

        groups[1].motor[RIGHT_MOTOR_TOP] = true;
        groups[1].motor[RIGHT_MOTOR_BOT] = true;
        strcpy(groups[1].name, "Right Drive");

        groups[2].motor[LEFT_MOTOR_TOP] = true;
        groups[2].motor[LEFT_MOTOR_BOT] = true;
        groups[2].motor[RIGHT_MOTOR_TOP] = true;
        groups[2].motor[RIGHT_MOTOR_BOT] = true;
        strcpy(groups[2].name, "Full Drive");

        groups[3].motor[NAUTILUS_SHOOTER_MOTOR_LEFT] = true;
        groups[3].motor[NAUTILUS_SHOOTER_MOTOR_RIGHT] = true;
        groups[3].motor[NAUTILUS_SHOOTER_MOTOR_CENTER] = true;
        strcpy(groups[3].name, "Nautilus Shooter");

        groups[4].motor[INTAKE_ROLLER_MOTOR] = true;
        strcpy(groups[4].name, "Intake");

        groups[5].motor[TRANSMISSION_MOTOR] = true;
        strcpy(groups[5].name, "Transmission");

        //saveGroups();
    } else {
        //loadGroups();
    }
}

/** 
 * Formats the LCD diagnostic menu name in the center of the screen.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 * @param line the line on the LCD to print the name
 * @param index the ID number of the menu
 */
void formatMenuNameCenter(FILE* lcdport, int line, int index){
    int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(menuChoices[index]))/2;
    char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
    for(int i = 0; i < spaces; i++){
        strcat(str, " ");
    }
    strcat(str, menuChoices[index]);
    for(int i = 0; i < spaces; i++){
        strcat(str, " ");
    }
    lcdSetText(lcdport, line, str);
}

/** 
 * Displays the list of menus and waits for the user to select one.
 * 
 * @return the ID number of the menu selected
 */
int selectMenu() {
    bool done = false;
    int val = 0;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        if(rightPressed && val != LCD_MENU_COUNT-1) val++;
        else if(rightPressed && val == LCD_MENU_COUNT-1) val = 0;
        else if(leftPressed && val != 0) val--;
        else if(leftPressed && val == 0) val = LCD_MENU_COUNT - 1;

        formatMenuNameCenter(LCD_PORT, 1, val);
        if(val == 0){
            lcdSetText(LCD_PORT, 2, "<      SEL     >");
        } else if(val == LCD_MENU_COUNT-1) {
            lcdSetText(LCD_PORT, 2, "<      SEL     >");
        } else {
            lcdSetText(LCD_PORT, 2, "<      SEL     >");
        }
        delay(20);
        done = centerPressed;
    } while(!done);
    printf("Selected menu choice: %d\n", val);
    return val;
}

/** 
 * Runs the screensaver that displays LCD messages.
 *
 * @see lcdmsg.c
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runScreensaver(FILE *lcdport){
    int cycle = 0;
    do {
        if(cycle == 0){
            screensaver(LCD_PORT);
        }
        delay(20);
        cycle++;
        if(cycle==150){
            cycle=0;
        }
    } while(!lcdAnyButtonPressed());
}

/** 
 * Displays the battery voltages, allowing for switching between batteries.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runBattery(FILE *lcdport){
    bool done = false;
    int val = BATT_MAIN;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        if(rightPressed && val != BATT_PEXP) val++;
        else if(rightPressed && val == BATT_PEXP) val = BATT_MAIN;
        else if(leftPressed && val != BATT_MAIN) val--;
        else if(leftPressed && val == BATT_MAIN) val = BATT_PEXP;

        int beforepoint = 0;
        int afterpoint = 0;
        char battdisp[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(battdisp, 0, sizeof(battdisp));
        memset(temp, 0, sizeof(temp));

        switch(val){
            case BATT_MAIN: beforepoint = powerLevelMain()/1000;
                            afterpoint = powerLevelMain()%1000;
                            strcat(battdisp, "Mn Batt: ");
                            break;
            case BATT_BKUP: beforepoint = powerLevelBackup()/1000;
                            afterpoint = powerLevelBackup()%1000;
                            strcat(battdisp, "Bk Batt: ");
                            break;
            case BATT_PEXP: beforepoint = powerLevelExpander()/1000;
                            afterpoint = powerLevelExpander()%1000;
                            strcat(battdisp, "Ex Batt: ");
                            break;
        }
        sprintf(temp, "%d.%d V", beforepoint, afterpoint);
        strcat(battdisp, temp);

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(battdisp))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1];
        memset(str, 0, sizeof(str));
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, battdisp);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        lcdSetText(LCD_PORT, 1, str);

        if(val == BATT_MAIN){
            lcdSetText(LCD_PORT, 2, "EX     ESC    BK");
        } else if(val == BATT_PEXP) {
            lcdSetText(LCD_PORT, 2, "BK     ESC    MN");
        } else { //BATT_BKUP
            lcdSetText(LCD_PORT, 2, "MN     ESC    EX");
        }
        delay(20);
        done = centerPressed;
    } while(!done);
}

/** 
 * Prompts the user to select an individual motor to test.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 * 
 * @return the motor number selected
 */
int selectMotor(FILE *lcdport){
    bool done = false;
    int val = 1;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        if(rightPressed && val != 10) val++;
        else if(rightPressed && val == 10) val = 0;
        else if(leftPressed && val != 0) val--;
        else if(leftPressed && val == 0) val = 10;

        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        if(val != 0){
            char temp[LCD_MESSAGE_MAX_LENGTH+1];
            memset(motorstr, 0, sizeof(motorstr));
            memset(temp, 0, sizeof(temp));
            strcpy(motorstr, "Motor: ");
            sprintf(temp, "%d", val);
            strcat(motorstr, temp);
        } else {
            strcpy(motorstr, "Cancel");
        }

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(motorstr))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, motorstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 1, str);

        done = centerPressed;
        if(val == 0){
            lcdSetText(LCD_PORT, 2, "10     SEL     1");
        } else if(val == 1) {
            lcdSetText(LCD_PORT, 2, "ESC    SEL     2");
        } else if(val == 10) {
            lcdSetText(LCD_PORT, 2, "9      SEL   ESC");
        } else if (val == 9) {
            lcdSetText(LCD_PORT, 2, "8      SEL    10");
        } else {
            char navstr[LCD_MESSAGE_MAX_LENGTH+1];
            memset(navstr, 0, sizeof(navstr));
            sprintf(navstr, "%c      SEL     %c",(val-1) + '0', (val+1) + '0');
            /*navstr[0] = (val-1) + '0';*/
            /*strcat(navstr, "      SEL     ");*/
            /*navstr[LCD_MESSAGE_MAX_LENGTH] = (val+1) + '0';*/
            lcdSetText(LCD_PORT, 2, navstr);
        }
        delay(20);
    } while(!done);
    printf("Testing motor %d.\n", val);
    return val;
}

/** 
 * Selects the speed of the motor to test.
 * 
 * @param mtr the motor number that is being tested
 * 
 * @return the speed selected
 */
int selectSpd(int mtr) {
    bool done = false;
    int val=0;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        /*bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);*/
        /*bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);*/

        val = (float) ((float) analogRead(AUTON_POT)/(float) AUTON_POT_HIGH) * 254;
        val -= 127;
        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));
        memset(temp, 0, sizeof(temp));
        strcpy(motorstr, "Motor: ");
        sprintf(temp, "%d", mtr);
        strcat(motorstr, temp);

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(motorstr))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, motorstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 1, str);

        char speedstr[LCD_MESSAGE_MAX_LENGTH+1];
        memset(speedstr, 0, sizeof(speedstr));
        memset(temp, 0, sizeof(temp));
        strcpy(speedstr, "Speed: ");
        sprintf(temp, "%d", val);
        strcat(speedstr, temp);

        spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(speedstr))/2;
        strcpy(str, "");
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, speedstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 2, str);

        done = (digitalRead(AUTON_BUTTON) == PRESSED || centerPressed);
        delay(20);
    } while(!done);
    printf("Using speed: %d\n", val);
    return val;
}

/** 
 * Runs the individual motor test.
 * Selection of the motor and speed is handled by other functions
 *
 * @see selectMotor()
 * @see selectSpd()
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runIndivMotor(FILE *lcdport){
    bool done = false;
    int mtr = selectMotor(lcdport);
    if(mtr == 0) return;
    int spd = selectSpd(mtr);
    int val = spd;
    disableOpControl = true;
    motorStopAll();
    do {
        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));
        memset(temp, 0, sizeof(temp));
        strcpy(motorstr, "Motor: ");
        sprintf(temp, "%d", mtr);
        strcat(motorstr, temp);

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(motorstr))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, motorstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 1, str);

        char speedstr[LCD_MESSAGE_MAX_LENGTH+1];
        memset(speedstr, 0, sizeof(speedstr));
        memset(temp, 0, sizeof(temp));
        strcpy(speedstr, "Run Speed: ");
        sprintf(temp, "%d", val);
        strcat(speedstr, temp);

        spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(speedstr))/2;
        strcpy(str, "");
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, speedstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 2, str);

        done = lcdAnyButtonPressed();
        motorSet(mtr, val);

        delay(20);
    } while(!done);
    disableOpControl = false;
}

/** 
 * Selects the motor group to test.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 * 
 * @return the ID number of the group to test
 */
int selectMotorGroup(FILE *lcdport){
    bool done = false;
    int val = 0;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        if(rightPressed && val != numgroups-1) val++;
        else if(rightPressed && val == numgroups-1) val = -1;
        else if(leftPressed && val != -1) val--;
        else if(leftPressed && val == -1) val = numgroups-1;

        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        if(val != -1){
            memset(motorstr, 0, sizeof(motorstr));
            strcpy(motorstr, groups[val].name);
        } else {
            strcpy(motorstr, "Cancel");
        }

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(motorstr))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, motorstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 1, str);

        done = centerPressed;
        lcdSetText(LCD_PORT, 2, "<      SEL     >");
        delay(20);
    } while(!done);
    return val;
}

/** 
 * Selects the speed of the motor group to be tested.
 * 
 * @param mtr the ID number of the motor group to be tested
 * 
 * @return the speed selected
 */
int selectSpdGroup(int mtr) {
    bool done = false;
    int val;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        /*bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);*/
        /*bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);*/

        val = (float) ((float) analogRead(AUTON_POT)/(float) AUTON_POT_HIGH) * 254;
        val -= 127;
        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));
        memset(temp, 0, sizeof(temp));
        strcpy(motorstr, groups[mtr].name);

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(motorstr))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, motorstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 1, str);

        char speedstr[LCD_MESSAGE_MAX_LENGTH+1];
        memset(speedstr, 0, sizeof(speedstr));
        memset(temp, 0, sizeof(temp));
        strcpy(speedstr, "Speed: ");
        sprintf(temp, "%d", val);
        strcat(speedstr, temp);

        spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(speedstr))/2;
        strcpy(str, "");
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, speedstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 2, str);

        done = (digitalRead(AUTON_BUTTON) == PRESSED || centerPressed);
        delay(20);
    } while(!done);
    return val;
}

/** 
 * Runs the motor group test.
 * Selection of the motor group and speed is handled by other functions.
 *
 * @see selectMotorGroup()
 * @see selectSpdGroup()
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runGroupMotor(FILE *lcdport){
    bool done = false;
    int mtr = selectMotorGroup(lcdport);
    if(mtr == -1) return;
    int spd = selectSpdGroup(mtr);
    int val = spd;
    disableOpControl = true;
    motorStopAll();
    do {
        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));
        memset(temp, 0, sizeof(temp));
        strcpy(motorstr, groups[mtr].name);

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(motorstr))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, motorstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 1, str);

        char speedstr[LCD_MESSAGE_MAX_LENGTH+1];
        memset(speedstr, 0, sizeof(speedstr));
        memset(temp, 0, sizeof(temp));
        strcat(speedstr, "Run Speed: ");
        sprintf(temp, "%d", val);
        strcat(speedstr, temp);

        spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(speedstr))/2;
        strcpy(str, "");
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, speedstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 2, str);

        for(int i = 1; i <= 10; i++){
            if(groups[mtr].motor[i]){
                motorSet(i, val);
            }
        }

        done = lcdAnyButtonPressed();
        delay(20);
    } while(!done);
    disableOpControl = false;
}

/** 
 * Runs the top-level motor testing menu.
 * Prompts the user to select between individual and group motor testing.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runMotor(FILE *lcdport){
    bool done = false;
    int val = 0;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        if(rightPressed && val != 1) val++;
        else if(leftPressed && val != 0) val--;

        if(val){
            lcdSetText(lcdport, 1, "Indiv Motor Test");
        } else {
            lcdSetText(lcdport, 1, "Group Motor Test");
        }
        done = centerPressed;
        if(val == 0){
            lcdSetText(LCD_PORT, 2, "|      SEL     >");
        } else if(val == 1) {
            lcdSetText(LCD_PORT, 2, "<      SEL     |");
        }
        delay(20);
    } while(!done);
    if(val){
        runIndivMotor(lcdport);
    } else {
        runGroupMotor(lcdport);
    }
}

/** 
 * Selects the motors that constitute the specified motor group.
 * 
 * @param motor a boolean array that stores the states of each motor in the group
 * 
 * @return a pointer to the array passed
 */
bool* selectMotorGroupMembers(bool *motor){
    bool done = false;
    int val = 1;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        if(rightPressed && val != 10) val++;
        else if(rightPressed && val == 10) val = 0;
        else if(leftPressed && val != 0) val--;
        else if(leftPressed && val == 0) val = 10;

        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        if(val != 0){
            char temp[LCD_MESSAGE_MAX_LENGTH+1];
            memset(motorstr, 0, sizeof(motorstr));
            memset(temp, 0, sizeof(temp));
            strcpy(motorstr, "Motor ");
            sprintf(temp, "%d:", val);
            strcat(motorstr, temp);
            if(motor[val]){
                strcat(motorstr, " On");
            } else {
                strcat(motorstr, " Off");
            }
        } else {
            strcpy(motorstr, "Confirm");
        }
        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(motorstr))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, motorstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 1, str);

        done = (centerPressed && val == 0);
        if(centerPressed && val != 0){
            motor[val] = !motor[val];
        }
        if(val == 0){
            lcdSetText(LCD_PORT, 2, "10     SEL     1");
        } else if(val == 1) {
            lcdSetText(LCD_PORT, 2, "ESC    SEL     2");
        } else if(val == 10) {
            lcdSetText(LCD_PORT, 2, "9      SEL   ESC");
        } else if (val == 9) {
            lcdSetText(LCD_PORT, 2, "8      SEL    10");
        } else {
            char navstr[LCD_MESSAGE_MAX_LENGTH+1];
            memset(navstr, 0, sizeof(navstr));
            sprintf(navstr, "%c      SEL     %c",(val-1) + '0', (val+1) + '0');
            /*navstr[0] = (val-1) + '0';*/
            /*strcat(navstr, "      SEL     ");*/
            /*navstr[LCD_MESSAGE_MAX_LENGTH] = (val+1) + '0';*/
            lcdSetText(LCD_PORT, 2, navstr);
        }
        delay(20);
    } while(!done);
    return motor;
}

/** 
 * Adds a new motor group to the dynamic array.
 */
void addMotorGroup(){
    MotorGroup *temp = (MotorGroup*) realloc(groups, sizeof(MotorGroup)*(numgroups+1));
    if(temp == NULL) return;
    groups = temp;
    numgroups++;
    memset(&groups[numgroups-1], 0, sizeof(groups[numgroups-1]));
    typeString(groups[numgroups-1].name);
    selectMotorGroupMembers(groups[numgroups-1].motor);
}

/** 
 * Edits a motor group.
 * Prompts the user to either edit the name or the motors in a motor group.
 * 
 * @param mtr the ID number of the motor group to edit
 */
void editMotorGroup(int mtr){
    if(mtr == -1) return;
    bool done = false;
    int val = 0;
    FILE *lcdport = LCD_PORT;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        if(rightPressed && val != 1) val++;
        else if(leftPressed && val != 0) val--;

        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));

        if(val){
            strcpy(motorstr, "Edit Name");
        } else {
            strcpy(motorstr, "Edit Motors");
        }

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(motorstr))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, motorstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 1, str);

        done = centerPressed;
        if(val == 0){
            lcdSetText(LCD_PORT, 2, "|      SEL     >");
        } else if(val == 1) {
            lcdSetText(LCD_PORT, 2, "<      SEL     |");
        }
        delay(20);
    } while(!done);
    if(val){
        typeString(groups[mtr].name);
    } else {
        selectMotorGroupMembers(groups[mtr].motor);
    }
}

/** 
 * Deletes a motor group.
 * Prompts the user whether to cancel or to proceed with deletion.
 * 
 * @param mtr the ID number to delete
 */
void delMotorGroup(int mtr){
    if(mtr == -1) return;
    int val = 0;
    FILE *lcdport = LCD_PORT;
    bool done = false;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        if(rightPressed && val != 1) val++;
        else if(leftPressed && val != 0) val--;

        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));

        if(val){
            strcpy(motorstr, "Delete Forever");
        } else {
            strcpy(motorstr, "Cancel Delete");
        }

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(motorstr))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, motorstr);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 1, str);

        done = centerPressed;
        if(val == 0){
            lcdSetText(LCD_PORT, 2, "|      SEL     >");
        } else if(val == 1) {
            lcdSetText(LCD_PORT, 2, "<      SEL     |");
        }
        delay(20);
    } while(!done);
    if(val){
        memset(&groups[mtr], 0, sizeof(groups[mtr]));
        for(int i = mtr; i < numgroups - 1; i++){
            memcpy(&groups[i], &groups[i+1], sizeof(groups[i]));
        }
        numgroups--;
    }
}

/** 
 * Runs the top-level motor group management menu.
 * Prompts the user whether to add, edit, or delete motor groups.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runMotorGroupMgmt(FILE *lcdport){
    bool done = false;
    int val = 0;
    do {
        bool centerPressed = lcdButtonPressed(LCD_BTN_CENTER);
        bool leftPressed = lcdButtonPressed(LCD_BTN_LEFT);
        bool rightPressed = lcdButtonPressed(LCD_BTN_RIGHT);

        if(rightPressed && val != 3) val++;
        else if(rightPressed && val == 3) val = 0;
        else if(leftPressed && val != 0) val--;
        else if(leftPressed && val == 0) val = 3;

        switch(val){
            case 0: lcdSetText(lcdport, 1, "Add Motor Group"); break;
            case 1: lcdSetText(lcdport, 1, "Edit Motor Group"); break;
            case 2: lcdSetText(lcdport, 1, "Del Motor Group"); break;
            case 3: lcdSetText(lcdport, 1, "Cancel Grp. Mgmt"); break;
        }
        done = centerPressed;
        if(val == 0){
            lcdSetText(LCD_PORT, 2, "<      SEL     >");
        } else {
            lcdSetText(LCD_PORT, 2, "<      SEL     >");
        }
        delay(20);
    } while(!done);
    switch(val){
        case 0: addMotorGroup(); break;
        case 1: editMotorGroup(selectMotorGroup(lcdport)); break;
        case 2: delMotorGroup(selectMotorGroup(lcdport)); break;
        case 3: break;
    }
}

/** 
 * Runs the joystick connection debugging menu.
 * Prints whether the main and partner joysticks are connected.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runConnection(FILE *lcdport){
    do {
        char strjoy1[LCD_MESSAGE_MAX_LENGTH+1] = "";
        char strjoy2[LCD_MESSAGE_MAX_LENGTH+1] = "";
        if(isJoystickConnected(1)){
            strcat(strjoy1, "J1: Connected");
        } else {
            strcat(strjoy1, "J1: Disconnected");
        }
        if(isJoystickConnected(2)){
            strcat(strjoy2, "J2: Connected");
        } else {
            strcat(strjoy2, "J2: Disconnected");
        }

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(strjoy1))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, strjoy1);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 1, str);

        spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(strjoy2))/2;
        strcpy(str, "");
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, strjoy2);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 2, str);

        delay(20);
    } while(!lcdAnyButtonPressed());
}

/** 
 * Runs the robot sensory information menu.
 * Displays information regarding competition switch status and gyroscope angle.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runRobot(FILE *lcdport){
    do {
        char strjoy1[LCD_MESSAGE_MAX_LENGTH+1] = "";
        char strjoy2[LCD_MESSAGE_MAX_LENGTH+1] = "";
        /*if(isOnline()){
            strcat(strjoy1, "Competition Mode");
        } else {
            strcat(strjoy1, "Practice Mode");
        }*/
        /*if(isEnabled()){
            strcat(strjoy2, "Robot Enabled");
        } else {
            strcat(strjoy2, "Robot Disabled");
        }*/
        snprintf(strjoy1, LCD_MESSAGE_MAX_LENGTH+1, "L: %d, R: %d", encoderGet(leftenc), encoderGet(rightenc));
        sprintf(strjoy2, "Angle: %d", gyroGet(gyro));

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(strjoy1))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, strjoy1);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 1, str);

        spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(strjoy2))/2;
        strcpy(str, "");
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, strjoy2);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 2, str);

        delay(20);
    } while(!lcdAnyButtonPressed());
}

/** 
 * Runs the autonomous recorder status menu.
 * Displays the autonomous that is currently loaded, and if controller playback is enabled.
 * Controller playback is automatically disabled when plugged into the competition switch.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runAuton(FILE *lcdport){
    do {
        char strjoy1[LCD_MESSAGE_MAX_LENGTH+1] = "";
        char strjoy2[LCD_MESSAGE_MAX_LENGTH+1] = "";
        if(autonLoaded == -1){
            strcat(strjoy1, "No Auton Loaded");
        } else if(autonLoaded == 0){
            strcat(strjoy1, "Empty Auton");
        } else if(autonLoaded == MAX_AUTON_SLOTS + 1){
            strcat(strjoy1, "Prog. Skills");
        } else {
            FILE* autonFile;
            char filename[AUTON_FILENAME_MAX_LENGTH];
            snprintf(filename, sizeof(filename)/sizeof(char), "a%d", autonLoaded);
            autonFile = fopen(filename, "r");
            char name[LCD_MESSAGE_MAX_LENGTH+1];
            memset(name, 0, sizeof(name));
            fread(name, sizeof(char), sizeof(name) / sizeof(char), autonFile);
            strcpy(strjoy1, name);
            fclose(autonFile);
        }
        if(isOnline()){
            strcat(strjoy2, "Recorder Off");
        } else {
            strcat(strjoy2, "Recorder On");
        }

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(strjoy1))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, strjoy1);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 1, str);

        spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(strjoy2))/2;
        strcpy(str, "");
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, strjoy2);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 2, str);

        delay(20);
    } while(!lcdAnyButtonPressed());
}

/** 
 * Runs the credits menu.
 * The LCD diagnostic menu was inspired by Team 750W and Akram Sandhu.
 * This would not be possible without their generosity and permissiveness to use their idea.
 *
 * Note: the implementation of this feature is completely different between the two teams.
 * No code was reused from their implementation of the LCD diagnostic menu.
 * 
 * @param lcdport the LCD screen's port (either UART1 or UART2)
 */
void runCredits(FILE *lcdport){
    do {
        char strjoy1[LCD_MESSAGE_MAX_LENGTH+1] = "";
        char strjoy2[LCD_MESSAGE_MAX_LENGTH+1] = "";
        strcat(strjoy1, "Thanks Team 750W");
        strcat(strjoy2, "Akram Sandhu");

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(strjoy1))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, strjoy1);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 1, str);

        spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(strjoy2))/2;
        strcpy(str, "");
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, strjoy2);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(lcdport, 2, str);

        delay(20);
    } while(!lcdAnyButtonPressed());
}

/** 
 * Dispatcher function that executes the selected LCD diagnostic menu function.
 * This function is called with the result of selectMenu().
 * 
 * @see selectMenu()
 * 
 * @param choice the selected menu to run
 */
void doMenuChoice(int choice){
    switch(choice){
        case MENU_SCREENSAVER: runScreensaver(LCD_PORT); break;
        case MENU_BATTERY: runBattery(LCD_PORT); break;
        case MENU_MOTOR: runMotor(LCD_PORT); break;
        case MENU_MOTOR_MGMT: runMotorGroupMgmt(LCD_PORT); break;
        case MENU_CONNECTION: runConnection(LCD_PORT); break;
        case MENU_ROBOT: runRobot(LCD_PORT); break;
        case MENU_AUTON: runAuton(LCD_PORT); break;
        case MENU_BACKLIGHT: lcdSetBacklight(LCD_PORT, !backlight); backlight = !backlight; break;
        case MENU_CREDITS: runCredits(LCD_PORT); break;
    }
}

/**
 * Runs the LCD diagnostic menu task.
 * This thread executes concurrently with the operator control task.
 * The LCD diagnostic menu starts in screensaver mode.
 * Pressing any button cancels screensaver mode and enters the selection menu.
 *
 * @param ignore does nothing - required by task definition
 */
void formatLCDDisplay(void *ignore){
    lcdSetBacklight(LCD_PORT, backlight);
    doMenuChoice(MENU_SCREENSAVER);
    while(true){
        doMenuChoice(selectMenu());
    }
}

