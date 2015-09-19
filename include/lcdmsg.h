#ifndef LCDMSG_H_
#define LCDMSG_H_

#define LCD_PORT uart1

#define LCD_MESSAGE_MAX_LENGTH 16
extern char *lcdmsg[];
void randlcdmsg(FILE* lcdport, int line);

#endif
