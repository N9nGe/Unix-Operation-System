/* terminal.h - Defines used in interactions with the terminal 
 * 
 * Tony  1  10.20.2022   initialize the terminal 
 * Tony  2  11.26.2022   Improve the structure into multi_terminal case
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include"../lib.h"
#include"../types.h"
#include"keyboard.h"
#include"../syscall.h"

#define TERMINAL_BUF_SIZE 128
typedef struct terminal_t
{
    int id;    // ID: 1, 2, 3 for F1 F2 F3
    unsigned int buf[TERMINAL_BUF_SIZE]; // TERMINAL buffer
    uint32_t count; // terminal count, record the number of ascii in terminal buffer
    //CP5
    int task_counter; // Task counter for current terminal
    struct pcb_t* running_pcb; // pcb pointer to current process of this terminal
    int read_flag;    // used for terminal_read syscall
    int cursor_x;      // TODO: turn outside structure into this
    int cursor_y;
    int terminal_process_running;   // current running process type
    int terminal_shell_counter;     // how many shell running in the current terminal
    int pingping_flag;              // whether running pingpong 
    int fish_flag;                  // whether running fish
    int grep_flag;                  // whether running grep
} terminal_t;

/*Global variable shared between scheduled functions*/
extern terminal_t terminal[4];
extern int display_term;
extern int last_term;

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
