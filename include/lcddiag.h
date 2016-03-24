/** @file lcddiag.h
 * @brief Header file for LCD diagnostic menu functions and definitions
 *
 * This file contains function prototypes and definitions for the LCD diagnostic menu.
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
 * This file maintains constants and internal states regarding the menu's functionality.
 *
 * The idea behind this was inspired by Team 750W and Akram Sandhu.
 * Without them, this project would not be possible.
 *
 * Note: the implementation of this feature is completely different between the two teams.
 * No code was reused from their implementation of the LCD diagnostic menu.
 */

#ifndef LCDDIAG_H
#define LCDDIAG_H

/**
 * Checks if the specified LCD button is pressed.
 * This function's valid parameters are: 
 *     - LCD_BTN_LEFT
 *     - LCD_BTN_RIGHT
 *     - LCD_BTN_CENTER.
 *
 * This function waits for the specified button to be released before terminating.
 * Due to this, it can only be called once per loop iteration.
 * Its value should be stored in a boolean variable.
 *
 * @param btn the button to check
 *
 * @return true if pressed, false if not
 */
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

/**
 * Checks if the any LCD button is pressed.
 * This function waits for the button to be released before terminating.
 * Due to this, it can only be called once per loop iteration.
 * Its value should be stored in a boolean variable.
 *
 * @return true if pressed, false if not
 */
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

/**
 * The number of top-level menus available in the LCD diagnostic menu system.
 */
#define LCD_MENU_COUNT 9

/**
 * Menu ID number of the motor testing menu.
 */
#define MENU_MOTOR 0

/**
 * Menu ID number of the motor group manager.
 */
#define MENU_MOTOR_MGMT 1

/**
 * Menu ID number of the battery voltage readout.
 */
#define MENU_BATTERY 2

/**
 * Menu ID number of the joystick connection state indicator.
 */
#define MENU_CONNECTION 3

/**
 * Menu ID number of the robot sensory state indicator.
 */
#define MENU_ROBOT 4

/**
 * Menu ID number of the autonomous recorder status indicator.
 */
#define MENU_AUTON 5

/**
 * Menu ID number of the backlight toggle.
 */
#define MENU_BACKLIGHT 6

/**
 * Menu ID number of the LCD message screensaver.
 */
#define MENU_SCREENSAVER 7

/**
 * Menu ID number of the credits menu.
 * Thanks again to Team 750W and Akram Sandhu.
 */
#define MENU_CREDITS 8

/**
 * Stores the top-level menu names.
 */
extern char menuChoices[LCD_MENU_COUNT][LCD_MESSAGE_MAX_LENGTH+1];

/**
 * Object representing the LCD diagnostic menu task.
 * The LCD diagnostic menu runs in a separate thread from the operator control code.
 * The TaskHandle allows for pausing and resuming of the LCD diagnostic menu during autonomous recording.
 */
extern TaskHandle lcdDiagTask;

/**
 * @brief Represents a logical motor grouping, to be used when testing motors.
 *
 * Has flags for each motor that belongs to the group, as well as a 16-character name.
 */
typedef struct MotorGroup {
    /** 
     * Stores if each motor is contained in this group or not.
     *
     * This array contains 11 values. Element 0 is ignored. Elements 1-10 represent the respective motor ports.
     */
    bool motor[11];

    /** 
     * The name of the motor group.
     *
     * The name can be a maximum of 16 characters.
     * The buffer is 17 characters to hold the null terminator.
     */
    char name[LCD_MESSAGE_MAX_LENGTH+1];
} MotorGroup;

/**
 * Array that stores the motor groups.
 * As this is a dynamic array, creating and editing new motor groups is possible.
 * These motor groups are added to the array via the Motor Group Management menu.
 */
extern MotorGroup *groups;

/**
 * Stores the number of motor groups.
 * This is functionally identical to the size of the motor group array.
 */
extern int numgroups;

/**
 * Disables operator control loop during motor testing.
 * Since running motors is not thread safe, it is necessary to stop operator control of the motors during testing.
 */
extern bool disableOpControl;

/**
 * Initializes the motor groups array to contain the standard set of groups.
 * This includes: Left Drive, Right Drive, Full Drive, Nautilus Shooter, Intake, and Strafe Motor.
 */
void initGroups();

/**
 * Uses the LCD and the autonomous potentiometer to type a string.
 * This is used to name motor groups and autonomous recordings.
 * The maximum length of string this function can type is 16 characters.
 *
 * @param dest a buffer to store the typed string (must be at least 17 characters to hold null terminator)
 *
 * @return a pointer to the buffer
 */
char* typeString(char* dest);

/**
 * Runs the LCD diagnostic menu task.
 * The task starts in screensaver mode.
 * Pressing any button cancels screensaver mode and enters the selection menu.
 *
 * @param ignore does nothing - required by task definition
 */
void formatLCDDisplay(void* ignore);

#endif

