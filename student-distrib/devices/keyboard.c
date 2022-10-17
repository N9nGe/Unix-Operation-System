/* i8259.c - Functions to interact with the Keyboard interrupt device
 * 
 * Tony  1  10.14.2022
 */
#include "../lib.h"
#include "keyboard.h"
#include "../types.h"
#include "../i8259.h"
#include "../x86_desc.h"





/*Local Variable*/
char* key_buf[KEY_BUF_SIZE] = {0};

/* scancode set, divided into 4 sets*/
// CP1 : we only use a limited set 1
// No cap no shift
static char scancode_set_1[MAX_SET_SIZE] = { // 58
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0' /*backspace*/,
     '\0' /*tab*/, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0' /*\*/, 
     '\0' /*caps*/, 'a', 's','d', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0' /*enter*/, '\\', 
    /*left shift*/ 'z', 'x', 'c', 'v','b', 'n', 'm',',', '.', '/', '\0' /*right shift*/, '*', '\0', 
                                         ' '/*space*/, '\0'
};
// With cap, no shift
static char scancode_set_2[MAX_SET_SIZE] = {
    '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
	 'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0s'
};
// no cap, with shift
static char scancode_set_3[MAX_SET_SIZE] = {
    '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V',
	 'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'
};
// with cap, with shift
static char scancode_set_4[MAX_SET_SIZE] = {
    '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', '\0', '\\', 'z', 'x', 'c', 'v',
	 'b', 'n', 'm', '<', '>', '?', '\0', '*', '\0', ' ', '\0'
};
/* Initialize keyboard input device */
void keyboard_init(void){
    printf("initialize keyboard...");
    enable_irq( KEYBOARD_IRQ_NUM );
    return;
};

void keyboard_interrupt_handler( void){
    
    cli(); // Clear all the interrupt first
    send_eoi(KEYBOARD_IRQ_NUM);
    int8_t scan_button = inb(KEYBOARD_PORT) & 0xff;
    char key_print = 'd';

// test if the button has special usage
    if (function_button_handle(scan_button) == 1) {
        sti();
        return;
    }
    if(scan_button > 0x01 && scan_button < MAX_SCAN_SIZE){
        if (scan_button >= 'a' && scan_button <= 'z')
        {   
            key_print = scancode_set_1[scan_button];
            putc(key_print);
        }
        
    }
    
    clear_keyboard_buffer();
    sti();
    return;
};


void clear_keyboard_buffer(void){
    int i;

    for ( i = 0; i < KEY_BUF_SIZE; i++)
    {
        key_buf[i] = KEY_NULL;
    }
    

    return;

};




int function_button_handle(int8_t scan_button){
    int ret = 0; 
    switch (scan_button)
    {
    case LEFT_SHIFT_PRESSED:
        ret = 1;
        break;
    case LEFT_SHIFT_RELEASED:
        ret = 1;
        break;

    case RIGHT_SHIFT_PRESSED:
        ret = 1;
        break;
    case RIGHT_SHIFT_RELEASED:
        ret = 1;
        break;
    case LEFT_CTRL_PRESSED:
        ret = 1;
        break;
    case LEFT_CTRL_RELEASED:
        ret = 1;
        break;    
    default:
    
        break;
    }
    return ret;
}
