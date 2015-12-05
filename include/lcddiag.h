#ifndef LCDDIAG_H
#define LCDDIAG_H

inline bool lcdButtonPressed(int btn){
    if(lcdReadButtons(LCD_PORT) & btn){
        do{
            delay(20);
        } while (lcdReadButtons(LCD_PORT) & btn);
        return true;
    } else {
        return false;
    }
    return true;
}

inline bool lcdAnyButtonPressed(){
    if(lcdReadButtons(LCD_PORT)){
        do{
            delay(20);
        } while (lcdReadButtons(LCD_PORT));
    } else {
        return false;
    }
    return true;
}

#define LCD_MENU_COUNT 9

#define MENU_MOTOR 0
#define MENU_MOTOR_MGMT 1
#define MENU_BATTERY 2
#define MENU_CONNECTION 3
#define MENU_ROBOT 4
#define MENU_AUTON 5
#define MENU_BACKLIGHT 6
#define MENU_SCREENSAVER 7
#define MENU_CREDITS 8

extern char menuChoices[LCD_MENU_COUNT][LCD_MESSAGE_MAX_LENGTH+1];

extern TaskHandle lcdDiagTask;

typedef struct MotorGroup {
    bool motor[11];
    char name[LCD_MESSAGE_MAX_LENGTH+1];
} MotorGroup;

extern MotorGroup *groups;
extern int numgroups;

extern bool disableOpControl;

void initGroups();
char* typeString();
void formatMsgCenter(FILE* lcdport, int line);
void formatLCDDisplay(void *ignore);


#endif
