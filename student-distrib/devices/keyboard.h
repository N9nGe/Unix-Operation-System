/* keyboard.h - Defines used in interactions with the Keyboard interrupt
 * 
 * Tony  1  10.14.2022
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "../types.h"
#include "../paging.h"

/* Ports that each PIC sits on */
#define KEYBOARD_IRQ 1
#define KEYBOARD_PORT 0x60

#define KEY_BUF_SIZE        128
#define KEY_NULL           '\0'
#define INITIAL_KEY         0x01
#define KEYBOARD_IRQ_NUM 0x01
#define MAX_SCAN_SIZE 58
#define MAX_SET_SIZE 100
#define MAX_INPUT_COUNT 100000
#define MAX_TERMINAL_NUMBER 3

#define VALID_RET 0
#define INVALID_RET 1 
// For checkpoint 2
#define SINGLE_QUATE      39 
#define DOUBLE_QUATE      34
/*Reference to any scancode of linux*/
/*op: showkey -s, and test by yourself*/
#define LEFT_SHIFT_PRESSED 0x2A
#define LEFT_SHIFT_RELEASED 0xAA
#define RIGHT_SHIFT_PRESSED 0x36
#define RIGHT_SHIFT_RELEASED 0xB6
#define CTRL_PRESSED 0x1D
#define CTRL_RELEASED 0x9D
// The left and right control shares the same id
#define ALT_PRESSED      0x38
#define ALT_RELEASED     (ALT_PRESSED + 0x80)
#define CAPSLOCK_PRESSED 0x3A
#define CAPSLOCK_RELEASED 0xBA
//CP5
#define F1      0x3b
#define F2      0x3c
#define F3      0x3d

/* Externally-visible functions */

extern uint8_t keyboard_buf[KEY_BUF_SIZE];
extern int     keybuf_count;
extern volatile int   write_flag;

/* Initialize keyboard input device */
void keyboard_init(void);

/* Keyboard Handler */
void keyboard_interrupt_handler(void);
/* Function key handler*/
int function_key_handle(unsigned int key);
/*reset the key buffer and function key state bit to initial state*/
void reset_keyboard_buffer(void);
/*Handle backspace, modify corresponding keyboard buffer*/
void backspace_handler();
//CP5: terminal context switch 
int terminal_switch(unsigned int key);


//CP4: Tony delete the open and close for keyboard driver,
// because in fact they are established through terminal driver
// in our MentOS

#endif /* _KEYBOARD_H */
