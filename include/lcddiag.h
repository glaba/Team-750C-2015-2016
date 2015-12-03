#ifndef LCDDIAG_H
#define LCDDIAG_H

inline bool lcdButtonPressed(int btn){
    if(lcdReadButtons(LCD_PORT) & btn){
        do{
            delay(20);
        } while (lcdReadButtons(LCD_PORT) & btn);
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

//inline char* itoa(int x, char *dest, int base){
    //sprintf(dest, "%d", x);
    //return dest;
//}

#define LCD_MENU_COUNT 9

#define MENU_SCREENSAVER 0
#define MENU_BATTERY 1
#define MENU_MOTOR 2
#define MENU_MOTOR_MGMT 3
#define MENU_CONNECTION 4
#define MENU_ROBOT 5
#define MENU_AUTON 6
#define MENU_BACKLIGHT 7
#define MENU_CREDITS 8

extern char menuChoices[LCD_MENU_COUNT][LCD_MESSAGE_MAX_LENGTH+1];
extern int currentMenu;

extern TaskHandle lcdDiagTask;

typedef struct MotorGroup {
    bool motor[11];
    char name[LCD_MESSAGE_MAX_LENGTH+1];
} MotorGroup;

extern MotorGroup *groups;
extern int numgroups;

void initGroups();
char* typeString();
void formatMsgCenter(FILE* lcdport, int line);
void formatLCDDisplay(void *ignore);

#endif
