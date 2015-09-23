#include "main.h"

/** 
 * Master list of all LCD messages.
 */
char *lcdmsg[] = {
    "Hint of Lime",
    "Review Committee",
    "Exacerbate",
    "Get Schwifty",
    "Hot Knife",
    "Mikel",
    "Michael",
    "Donald J. Trump",
    "Nautilus Gears",
    "Drop and Pop"
};

/** 
 * Displays a random LCD message from the master list.
 * 
 * @param lcdport the port the LCD is connected to
 * @param line the line to display the message on
 */
void randlcdmsg(FILE *lcdport, int line){
    int index = rand() % LCD_MESSAGE_COUNT;
    int spaces = (LCD_MESSAGE_MAX_LENGTH - strlen(lcdmsg[index]))/2;
    char str[LCD_MESSAGE_MAX_LENGTH+1] = "";
    for(int i = 0; i < spaces; i++){
        strcat(str, " ");
    }
    strcat(str, lcdmsg[index]);
    for(int i = 0; i < spaces; i++){
        strcat(str, " ");
    }
    lcdSetText(lcdport, line, str);
}

/** 
 * Formats the LCD by displaying 750C title and message.
 * 
 * @param lcdport the port the LCD is connected to
 */
void formatLCDDisplay(FILE *lcdport) {
    lcdSetText(lcdport, 1, LCD_750C_TITLE);
    randlcdmsg(lcdport, 2);
}
