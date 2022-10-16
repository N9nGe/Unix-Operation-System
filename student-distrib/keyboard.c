/* i8259.c - Functions to interact with the Keyboard interrupt device
 * 
 * Tony  1  10.14.2022
 */
#include "lib.h"
#include "keyboard.h"



#include "types.h"
#include "i8259.h"
#include "x86_desc.h"

#define KEYBOARD_IRQ_NUM 0x01
/*Local Variable*/
char* key_buf[KEY_BUF_SIZE] = {0};

/* scancode set, divided into 4 sets*/
// CP1 : we only use a limited set 1
// No cap no shift
static char scancode_set_1 = {
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0' /*backspace*/,
     '\0' /*tab*/, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0' /*\*/, 
     '\0' /*caps*/, 'a', 's','d', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0' /*enter*/, '\\', 
    /*left shift*/ 'z', 'x', 'c', 'v','b', 'n', 'm',',', '.', '/', '\0' /*right shift*/, '*', '\0', 
                                         ' '/*space*/, '\0'
};
// With cap, no shift
static char scancode_set_2 = {
    '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
	 'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0s'
};
// no cap, with shift
static char scancode_set_3 = {
    '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V',
	 'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'
};
// with cap, with shift
static char scancode_set_4 = {
    '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', '\0', '\\', 'z', 'x', 'c', 'v',
	 'b', 'n', 'm', '<', '>', '?', '\0', '*', '\0', ' ', '\0'
};
/* Initialize keyboard input device */
void keyboard_init(void){
    enable_irq( KEYBOARD_IRQ_NUM );
    return;
};

void keyboard_interrupt_handler( void){
    
    cli(); // Clear all the interrupt first
    
    
    clear_keyboard_buffer();
    sti();
    return;
};


void clear_keyboard_buffer(void){
    return;
};