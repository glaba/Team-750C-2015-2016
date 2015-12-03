#include "main.h"

TaskHandle lcdDiagTask = NULL;
bool backlight = true;
MotorGroup *groups;
int numgroups;

char menuChoices[LCD_MENU_COUNT][LCD_MESSAGE_MAX_LENGTH+1] = {
    "Screensaver",
    "Battery Info",
    "Motor Test",
    "Motor Group Mgmt",
    "Connection Info",
    "Robot Info",
    "Autonomous Info",
    "Toggle Backlight",
    "Credits"
};

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
        switch(c){
            case UPPER:
            case LOWER: mult = 28; spacecode = 26; endcode = 27; break;
            case NUMBER: mult = 12; spacecode = 10; endcode = 11; break;
        }
        val = (float) ((float) analogRead(AUTON_POT)/(float) AUTON_POT_HIGH) * mult;
        if(val == spacecode){
            dest[i] = ' ';
        } else if(val == endcode) {
            dest[i] = '^';
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
                lcdSetText(LCD_PORT, 2, "       SEL   abc");
            } else if(c == LOWER) {
                lcdSetText(LCD_PORT, 2, "       SEL   123");
            } else { //NUMBER
                lcdSetText(LCD_PORT, 2, "       SEL   ABC");
            }
        }

        if((lcdButtonPressed(LCD_BTN_CENTER) || digitalRead(AUTON_BUTTON) == PRESSED) && val != endcode){
            i++;
        } else if(lcdButtonPressed(LCD_BTN_LEFT) && i > 0){
            dest[i] = 0;
            i--;
        } else if(lcdButtonPressed(LCD_BTN_RIGHT)) {
            switch(c){
                case UPPER: c = LOWER; break;
                case LOWER: c = NUMBER; break;
                case NUMBER: c = UPPER; break;
            }
        }

        done = ((digitalRead(AUTON_BUTTON) == PRESSED && val == endcode)
                || (lcdButtonPressed(LCD_BTN_CENTER) && val == endcode)
                || i == LCD_MESSAGE_MAX_LENGTH);
        delay(20);
    } while(!done);
    dest[i] = 0;
    return dest;
}

void saveGroups(){
    FILE* group = fopen("groups", "w");
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

void loadGroups(){
    FILE* group = fopen("groups", "r");
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

void initGroups(){
    FILE* group = fopen("groups", "r");
    if(group == NULL){
        numgroups = 6; //LDRIVE, RDRIVE, DRIVE, SHOOT, INTK, TRANS 
        groups = (MotorGroup*) malloc(sizeof(MotorGroup) * numgroups);
        if(groups == NULL){
            return;
        }
        memset(groups, 0, sizeof(*groups));
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
#ifdef SHOOTER_HAS_THREE_MOTORS
        groups[3].motor[NAUTILUS_SHOOTER_MOTOR_CENTER] = true;
#endif
        strcpy(groups[3].name, "Nautilus Shooter");

        groups[4].motor[INTAKE_ROLLER_MOTOR] = true;
        groups[4].motor[INTAKE_CONVEYOR_MOTOR] = true;
        strcpy(groups[4].name, "Intake");

        groups[5].motor[TRANSMISSION_MOTOR] = true;
        strcpy(groups[5].name, "Transmission");

        saveGroups();
    } else {
        loadGroups();
    }
}

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

int selectMenu() {
    bool done = false;
    int val = 0;
    do {
        if(lcdButtonPressed(LCD_BTN_RIGHT) && val != LCD_MENU_COUNT-1) val++;
        else if(lcdButtonPressed(LCD_BTN_RIGHT) && val == LCD_MENU_COUNT-1) val = 0;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val != 0) val--;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val == 0) val = 0;

        formatMenuNameCenter(LCD_PORT, 1, val);
        done = lcdButtonPressed(LCD_BTN_CENTER);
        if(val == 0){
            lcdSetText(LCD_PORT, 2, "<      SEL     >");
        } else if(val == LCD_MENU_COUNT-1) {
            lcdSetText(LCD_PORT, 2, "<      SEL     >");
        } else {
            lcdSetText(LCD_PORT, 2, "<      SEL     >");
        }
        delay(20);
    } while(!done);
    printf("Selected menu choice: %d\n", val);
    return val;
}

void runScreensaver(FILE *lcdport){
    int cycle = 0;
    do {
        screensaver(LCD_PORT);
        delay(20);
        cycle++;
        if(cycle==150){
            cycle=0;
        }
    } while(!lcdAnyButtonPressed());
}

void runBattery(FILE *lcdport){
    bool done = false;
    int val = BATT_MAIN;
    do {
        if(lcdButtonPressed(LCD_BTN_RIGHT) && val != BATT_PEXP) val++;
        else if(lcdButtonPressed(LCD_BTN_RIGHT) && val == BATT_PEXP) val = BATT_MAIN;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val != BATT_MAIN) val--;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val == BATT_MAIN) val = BATT_PEXP;

        int beforepoint = 0;
        int afterpoint = 0;
        char battdisp[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(battdisp, 0, sizeof(battdisp));
        memset(temp, 0, sizeof(temp));

        switch(val){
            case BATT_MAIN: beforepoint = powerLevelMain()/1000;
                            afterpoint = powerLevelMain()%1000;
                            strcat(battdisp, "Mn. Batt: ");
                            break;
            case BATT_BKUP: beforepoint = powerLevelBackup()/1000;
                            afterpoint = powerLevelBackup()%1000;
                            strcat(battdisp, "Bk. Batt: ");
                            break;
            case BATT_PEXP: beforepoint = powerLevelExpander()/1000;
                            afterpoint = powerLevelExpander()%1000;
                            strcat(battdisp, "Ex. Batt: ");
                            break;
        }
        sprintf(temp, "%d", beforepoint);
        strcat(battdisp, temp);
        strcat(battdisp, ".");
        sprintf(temp, "%d", afterpoint);
        strcat(battdisp, temp);
        strcat(battdisp, " V");

        int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(battdisp))/2;
        char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }
        strcat(str, battdisp);
        for(int i = 0; i < spaces; i++){
            strcat(str, " ");
        }

        lcdSetText(LCD_PORT, 1, str);

        done = lcdButtonPressed(LCD_BTN_CENTER);
        if(val == BATT_MAIN){
            lcdSetText(LCD_PORT, 2, "EX     ESC    BK");
        } else if(val == BATT_PEXP) {
            lcdSetText(LCD_PORT, 2, "BK     ESC    MN");
        } else { //BATT_BKUP
            lcdSetText(LCD_PORT, 2, "MN     ESC    EX");
        }
        delay(20);
    } while(!done);
}

int selectMotor(FILE *lcdport){
    bool done = false;
    int val = 1;
    do {
        if(lcdButtonPressed(LCD_BTN_RIGHT) && val != 10) val++;
        else if(lcdButtonPressed(LCD_BTN_RIGHT) && val == 10) val = 0;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val != 0) val--;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val == 0) val = 10;

        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        if(val != 0){
            char temp[LCD_MESSAGE_MAX_LENGTH+1];
            memset(motorstr, 0, sizeof(motorstr));
            memset(temp, 0, sizeof(temp));
            strcat(motorstr, "Motor: ");
            sprintf(temp, "%d", val);
            strcat(motorstr, temp);
        } else {
            strcat(motorstr, "Cancel");
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

        done = lcdButtonPressed(LCD_BTN_CENTER);
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
            navstr[0] = (val-1) + '0';
            strcat(navstr, "      SEL     0");
            navstr[LCD_MESSAGE_MAX_LENGTH] = (val+1) + '0';
            lcdSetText(LCD_PORT, 2, navstr);
        }
        delay(20);
    } while(!done);
    return val;
}

int selectSpd(int mtr) {
    bool done = false;
    int val;
    do {
        val = (float) ((float) analogRead(AUTON_POT)/(float) AUTON_POT_HIGH) * 254;
        val -= 127;
        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));
        memset(temp, 0, sizeof(temp));
        strcat(motorstr, "Motor: ");
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
        strcat(motorstr, "Speed: ");
        sprintf(temp, "%d", val);
        strcat(motorstr, temp);

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

        done = (digitalRead(AUTON_BUTTON) == PRESSED || lcdButtonPressed(LCD_BTN_CENTER));
        delay(20);
    } while(!done);
    return val;
}

void runIndivMotor(FILE *lcdport){
    bool done = false;
    int mtr = selectMotor(lcdport);
    if(mtr == 0) return;
    int spd = selectSpd(mtr);
    int val = spd;
    motorStopAll();
    taskPrioritySet(NULL, TASK_PRIORITY_HIGHEST-1);
    do {
        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));
        memset(temp, 0, sizeof(temp));
        strcat(motorstr, "Motor: ");
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
        strcat(motorstr, "Speed: ");
        sprintf(temp, "%d", val);
        strcat(motorstr, temp);

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

        motorSet(mtr, val);

        done = lcdAnyButtonPressed();
        //delay(20);
    } while(!done);
    taskPrioritySet(NULL, TASK_PRIORITY_DEFAULT);
}

int selectMotorGroup(FILE *lcdport){
    bool done = false;
    int val = 0;
    do {
        if(lcdButtonPressed(LCD_BTN_RIGHT) && val != numgroups-1) val++;
        else if(lcdButtonPressed(LCD_BTN_RIGHT) && val == numgroups-1) val = -1;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val != -1) val--;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val == -1) val = numgroups-1;

        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        if(val != -1){
            memset(motorstr, 0, sizeof(motorstr));
            strcat(motorstr, groups[val].name);
        } else {
            strcat(motorstr, "Cancel");
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

        done = lcdButtonPressed(LCD_BTN_CENTER);
        lcdSetText(LCD_PORT, 2, "<      SEL     >");
        delay(20);
    } while(!done);
    return val;
}

int selectSpdGroup(int mtr) {
    bool done = false;
    int val;
    do {
        val = (float) ((float) analogRead(AUTON_POT)/(float) AUTON_POT_HIGH) * 254;
        val -= 127;
        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));
        memset(temp, 0, sizeof(temp));
        strcat(motorstr, groups[mtr].name);

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
        strcat(motorstr, "Speed: ");
        sprintf(temp, "%d", val);
        strcat(motorstr, temp);

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

        done = (digitalRead(AUTON_BUTTON) == PRESSED || lcdButtonPressed(LCD_BTN_CENTER));
        delay(20);
    } while(!done);
    return val;
}

void runGroupMotor(FILE *lcdport){
    bool done = false;
    int mtr = selectMotorGroup(lcdport);
    if(mtr == -1) return;
    int spd = selectSpdGroup(mtr);
    int val = spd;
    motorStopAll();
    taskPrioritySet(NULL, TASK_PRIORITY_HIGHEST-1);
    do {
        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        char temp[LCD_MESSAGE_MAX_LENGTH+1];
        memset(motorstr, 0, sizeof(motorstr));
        memset(temp, 0, sizeof(temp));
        strcat(motorstr, "Motor: ");
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
        strcat(motorstr, "Speed: ");
        sprintf(temp, "%d", val);
        strcat(motorstr, temp);

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
        //delay(20);
    } while(!done);
    taskPrioritySet(NULL, TASK_PRIORITY_DEFAULT);
}

void runMotor(FILE *lcdport){
    bool done = false;
    int val = 0;
    do {
        if(lcdButtonPressed(LCD_BTN_RIGHT) && val != 1) val++;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val != 0) val--;

        if(val){
            lcdSetText(lcdport, 1, "Indiv Motor Test");
        } else {
            lcdSetText(lcdport, 1, "Group Motor Test");
        }
        done = lcdButtonPressed(LCD_BTN_CENTER);
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

bool* selectMotorGroupMembers(bool *motor){
    bool done = false;
    int val = 1;
    do {
        if(lcdButtonPressed(LCD_BTN_RIGHT) && val != 10) val++;
        else if(lcdButtonPressed(LCD_BTN_RIGHT) && val == 10) val = 0;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val != 0) val--;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val == 0) val = 10;

        char motorstr[LCD_MESSAGE_MAX_LENGTH+1];
        if(val != 0){
            char temp[LCD_MESSAGE_MAX_LENGTH+1];
            memset(motorstr, 0, sizeof(motorstr));
            memset(temp, 0, sizeof(temp));
            strcat(motorstr, "Motor ");
            sprintf(temp, "%d:", val);
            strcat(motorstr, temp);
            if(motor[val]){
                strcat(motorstr, " On");
            } else {
                strcat(motorstr, " Off");
            }
        } else {
            strcat(motorstr, "Confirm");
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

        done = (lcdButtonPressed(LCD_BTN_CENTER) && val == 0);
        if(lcdButtonPressed(LCD_BTN_CENTER) && val != 0){
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
            navstr[0] = (val-1) + '0';
            strcat(navstr, "      SEL     0");
            navstr[LCD_MESSAGE_MAX_LENGTH] = (val+1) + '0';
            lcdSetText(LCD_PORT, 2, navstr);
        }
        delay(20);
    } while(!done);
    return motor;
}

void addMotorGroup(){
    MotorGroup *temp = (MotorGroup*) realloc(groups, sizeof(MotorGroup)*(numgroups+1));
    if(temp == NULL) return;
    groups = temp;
    numgroups++;
    memset(&groups[numgroups-1], 0, sizeof(groups[numgroups-1]));
    typeString(groups[numgroups-1].name);
    selectMotorGroupMembers(groups[numgroups-1].motor);
}

void editMotorGroup(int mtr){
    if(mtr == -1) return;
    bool done = false;
    int val = 0;
    FILE *lcdport = LCD_PORT;
    do {
        if(lcdButtonPressed(LCD_BTN_RIGHT) && val != 1) val++;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val != 0) val--;

        if(val){
            lcdSetText(lcdport, 1, "Edit Name");
        } else {
            lcdSetText(lcdport, 1, "Edit Motors");
        }
        done = lcdButtonPressed(LCD_BTN_CENTER);
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

void delMotorGroup(int mtr){
    if(mtr == -1) return;
    int val = 0;
    FILE *lcdport = LCD_PORT;
    bool done = false;
    do {
        if(lcdButtonPressed(LCD_BTN_RIGHT) && val != 1) val++;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val != 0) val--;

        if(val){
            lcdSetText(lcdport, 1, "Delete Forever");
        } else {
            lcdSetText(lcdport, 1, "Cancel");
        }
        done = lcdButtonPressed(LCD_BTN_CENTER);
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

void runMotorGroupMgmt(FILE *lcdport){
    bool done = false;
    int val = 0;
    do {
        if(lcdButtonPressed(LCD_BTN_RIGHT) && val != 2) val++;
        else if(lcdButtonPressed(LCD_BTN_RIGHT) && val == 2) val = 0;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val != 0) val--;
        else if(lcdButtonPressed(LCD_BTN_LEFT) && val == 0) val = 2;

        switch(val){
            case 0: lcdSetText(lcdport, 1, "Add Motor Group"); break;
            case 1: lcdSetText(lcdport, 1, "Edit Motor Group"); break;
            case 2: lcdSetText(lcdport, 1, "Del Motor Group"); break;
        }
        done = lcdButtonPressed(LCD_BTN_CENTER);
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
    }
}

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

void runRobot(FILE *lcdport){
    do {
        char strjoy1[LCD_MESSAGE_MAX_LENGTH+1] = "";
        char strjoy2[LCD_MESSAGE_MAX_LENGTH+1] = "";
        if(isOnline()){
            strcat(strjoy1, "Competition Mode");
        } else {
            strcat(strjoy1, "Practice Mode");
        }
        if(isEnabled()){
            strcat(strjoy2, "Robot Enabled");
        } else {
            strcat(strjoy2, "Robot Disabled");
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

void runAuton(FILE *lcdport){
    do {
        char strjoy1[LCD_MESSAGE_MAX_LENGTH+1] = "";
        char strjoy2[LCD_MESSAGE_MAX_LENGTH+1] = "";
        if(autonLoaded == -1){
            strcat(strjoy1, "No Auton Loaded");
        } else if(autonLoaded == 0){
            strcat(strjoy1, "Blank Auton");
        } else {
            char temp[LCD_MESSAGE_MAX_LENGTH+1];
            memset(temp, 0, sizeof(temp));
            strcat(strjoy1, "Auton Slot: ");
            sprintf(temp, "%d", autonLoaded);
            strcat(strjoy1, temp);
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

void formatLCDDisplay(void *ignore){ //runs as task
    lcdSetBacklight(LCD_PORT, backlight);
    doMenuChoice(MENU_SCREENSAVER);
    while(true){
        doMenuChoice(selectMenu());
    }
}
