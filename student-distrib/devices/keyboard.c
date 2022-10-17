/* i8259.c - Functions to interact with the Keyboard interrupt device
 * 
 * Tony  1  10.14&15.2022 CP1 keyboard
 * Tony  2  10.16.2022    reconstuct the keyboard.c
 * 
 */
#include "../lib.h"
#include "keyboard.h"
#include "../types.h"
#include "../i8259.h"
#include "../x86_desc.h"

/* File Scope Variable*/

/* Set to avoid release repetition*/
int i = 0;            // #input counter 
unsigned int pre = 0; // buffer to record last input key

/*The CP1 edition scancode list*/
// CP1 : we only use a limited set 1
// No cap no shift

unsigned char scancode[58] = 
{   0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,
    0,'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';',39, '`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,0,0,
    '\0'
};




/* Initialize keyboard input device */
void keyboard_init(void){
    // printf("initialize keyboard...");
    enable_irq( KEYBOARD_IRQ_NUM );
    return;
};

void keyboard_interrupt_handler(){
    // printf("key pressed ");
    // cli(); // Clear all the interrupt first
    unsigned int key;
    unsigned int value;

    key = inb(KEYBOARD_PORT) & 0xff;
    value = scancode[key];
    
    if (function_key_handle(key) == 1){
        putc('\0'); // Default input for function key in CP1
    }else{
        if( (i%2 !=1) || pre == value){
            putc(value);
        }
    }
    send_eoi(KEYBOARD_IRQ_NUM); // end present interrupt
    i++;
    pre = value;
    if (i> MAX_INPUT_COUNT)
    {
        reset_keyboard_buffer();
    }
    
    return;
};
/* 
 * function_key_handle
 *  DESCRIPTION: a function port for further CP usage 
 *  INPUTS: key -- the input key value
 *  OUTPUTS: none
 *  RETURN VALUE: 
 *      VALID_RET   -- 0, if there is no function key pressed
 *      INVALID_RET -- 1, if there is function key pressed
 *  SIDE EFFECTS: none
 */
int function_key_handle(unsigned int key){
    int ret = VALID_RET; 
    switch (key)
    {
    case LEFT_SHIFT_PRESSED:
        ret = INVALID_RET;
        break;
    case LEFT_SHIFT_RELEASED:
        ret = INVALID_RET;
        break;

    case RIGHT_SHIFT_PRESSED:
        ret = INVALID_RET;
        break;
    case RIGHT_SHIFT_RELEASED:
        ret = INVALID_RET;
        break;
    case LEFT_CTRL_PRESSED:
        ret = INVALID_RET;
        break;
    case LEFT_CTRL_RELEASED:
        ret = INVALID_RET;
        break;    
    default:
    
        break;
    }
    return ret;
};
/* 
 * reset_keyboard_buffer
 *  DESCRIPTION: a helper function to reset the 
 *  keyboard buffer 
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: reset two global variable i and pre
 */
void reset_keyboard_buffer(){
    i = 0;
    pre = 0;
}
