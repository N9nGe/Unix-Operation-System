/* terminal.h - Defines used in interactions with the terminal 
 * 
 * Tony  1  10.20.2022   initialize the terminal 
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include"../lib.h"
#include"../types.h"
#include"keyboard.h"

#define TERMINAL_BUF_SIZE 128
typedef struct terminal_t
{
    int id;
    int index; // line buffer index&counter
    unsigned int buf[TERMINAL_BUF_SIZE];

} terminal_t;

extern terminal_t main_terminal;

void terminal_init();
void terminal_reset(terminal_t terminal);
int32_t terminal_read(int32_t fd, void* buf, uint32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);


#endif /* _TERMINAL_H */
