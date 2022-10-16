/* keyboard.h - Defines used in interactions with the Keyboard interrupt
 * 
 * Tony  1  10.14.2022
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H
//todo
#include "types.h"

/* Ports that each PIC sits on */



#define EOI                 0x60
#define KEY_BUF_SIZE        1
/* Externally-visible functions */

/* Initialize keyboard input device */
void keyboard_init(void);


/* Keyboard Handler */
void keyboard_interrupt_handler(void);

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
