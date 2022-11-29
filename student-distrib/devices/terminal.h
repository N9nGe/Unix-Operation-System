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
    int id;    // ID: 1, 2, 3 for F1 F2 F3
    int index; // line buffer index&counter
    unsigned int buf[TERMINAL_BUF_SIZE]; // TERMINAL buffer
    uint32_t count;
    int task_counter; // Task counter for current terminal
    int read_flag;  // keyboard flag 
} terminal_t;

extern terminal_t terminal[4];
extern int running_term;
extern int last_terminal;
// initalize the terminals
void terminal_init();
// reset the specific terminal structure 
void terminal_reset(terminal_t terminal);
// Load kb_buf -> buf specificed
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
// Load buf -> screen
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
// Open the file, uselss for CP2
int32_t terminal_open(const uint8_t* filename);
// Close the file, uselss for CP2
int32_t terminal_close(int32_t fd);


#endif /* _TERMINAL_H */
