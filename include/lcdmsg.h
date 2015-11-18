#ifndef LCDMSG_H_
#define LCDMSG_H_

/**
 * Defines the port the LCD is plugged into.
 */
#define LCD_PORT uart1

/**
 * Defines title string for LCD to display.
 */
#define LCD_750C_TITLE "  $$$ 750C $$$  "

/**
 * Defines the amount of LCD messages in the master list.
 */
#define LCD_MESSAGE_COUNT 36

/**
 * Defines the max length for LCD messages.
 */
#define LCD_MESSAGE_MAX_LENGTH 16

/** 
 * Master list of all LCD messages.
 */
extern char *lcdmsg[];

/** 
 * Displays a random LCD message from the master list.
 * 
 * @param lcdport the port the LCD is connected to
 * @param line the line to display the message on
 */
void randlcdmsg(FILE *lcdport, int line);

/** 
 * Formats the LCD by displaying 750C title and message.
 * 
 * @param lcdport the port the LCD is connected to
 */
void formatLCDDisplay(FILE *lcdport);

#endif
