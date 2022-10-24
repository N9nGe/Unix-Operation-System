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
extern uint32_t terminal_count;
// initalize the terminal
void terminal_init();
// reset the specific terminal structure 
void terminal_reset(terminal_t terminal);
// Load kb_buf -> buf specificed
int32_t terminal_read(int32_t fd, void* buf, uint32_t nbytes);
// Load buf -> screen
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
// Open the file, uselss for CP2
int32_t terminal_open(const uint8_t* filename);
// Close the file, uselss for CP2
int32_t terminal_close(int32_t fd);


#endif /* _TERMINAL_H */
