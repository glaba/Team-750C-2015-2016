#include "main.h"

char *lcdmsg[] = {
    "Hint of Lime",
    "Review Committee",
    "Exacerbate",
    "Get Schwifty",
    "Hot Knife",
    "Mikel",
    "Michael",
    "Donald J. Trump",
    "Nautilus Gears"
};

void randlcdmsg(FILE* lcdport, int line){
    int index = rand() % (sizeof lcdmsg/sizeof lcdmsg[0]);
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
