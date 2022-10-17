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
// Used for CP2
#define LEFT_SHIFT_PRESSED 0x2A
#define RIGHT_SHIFT_PRESSED 0x36
#define LEFT_SHIFT_RELEASED 0xAA
#define RIGHT_SHIFT_RELEASED 0xB6
#define LEFT_CTRL_PRESSED 0x1D
#define LEFT_CTRL_RELEASED 0x9D
#define CAPSLOCK_PRESSED 0x3A
#define CAPSLOCK_RELEASED 0xBA
#define ALT_PRESSED 0x38
#define ALT_RELEASED 0xB8
#define F1  0x3B
#define F2  0x3C
#define F3  0x3D
#define F4  0x3E
#define F5  0x3F
#define ESC 0x01
#define TAB_PRESSED 0x0F
#define UP_PRESSED 0x48
#define DOWN_PRESSED 0X50

/* Externally-visible functions */

/* Initialize keyboard input device */
void keyboard_init(void);


/* Keyboard Handler */
void keyboard_interrupt_handler(void);

int function_button_handle(char scan_button);

/* Called when enter key needs to be handled */
void handle_enter();

/* Called when backspace key needs to be handled*/
void handle_backspace();

/* Called when tab key needs to be handled*/
void handle_tab();

int process_char(int scode);

void clear_keyboard_buffer(void);

void clear_keyboard_backup(uint32_t term_id);

void read_buffer();
void show_keyboard_buffer();
void show_last_command();


#endif /* _KEYBOARD_H */
