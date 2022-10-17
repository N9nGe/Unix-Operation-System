/* keyboard.h - Defines used in interactions with the Keyboard interrupt
 * 
 * Tony  1  10.14.2022
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H
//todo
#include "../types.h"

/* Ports that each PIC sits on */
#define KEYBOARD_IRQ 1
#define KEYBOARD_PORT 0x60

#define KEY_BUF_SIZE        1
#define KEY_NULL           '\0'

#define KEYBOARD_IRQ_NUM 0x01
#define MAX_SCAN_SIZE 58
#define MAX_SET_SIZE 100

#define VALID_RET 0
#define INVALID_RET 1 // For checkpoint one 

#define LEFT_SHIFT_PRESSED 0x2A
#define LEFT_SHIFT_RELEASED 0xAA
#define RIGHT_SHIFT_PRESSED 0x36
#define RIGHT_SHIFT_RELEASED 0xB6
#define LEFT_CTRL_PRESSED 0x1D
#define LEFT_CTRL_RELEASED 0x9D
/* Externally-visible functions */

/* Initialize keyboard input device */
void keyboard_init(void);


/* Keyboard Handler */
void keyboard_interrupt_handler(void);
/* Function key handler*/
int function_key_handle(unsigned int key);
/*set the key buffer to initial state*/
void clear_keyboard_buffer(void);




#endif /* _KEYBOARD_H */
