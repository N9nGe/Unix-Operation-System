/* i8259.c - Functions to interact with the Keyboard interrupt device
 * 
 * Tony  1  10.14&15.2022 CP1 keyboard
 * Tony  2  10.16.2022    
 * 
 */
#include "../lib.h"
#include "keyboard.h"
#include "../types.h"
#include "../i8259.h"
#include "../x86_desc.h"


int i = 0;
unsigned int pre = 0;
unsigned char scancode[58] = 
{ 0,0,'1','2','3','4','5','6','7','8','9','0','-','=',
0,0,'q','w','e','r','t','y','u','i','o','p','[',']',
'\n',0,'a','s','d','f','g','h','j','k','l',';',39,
'`',0,'\\','z','x','c','v','b','n','m',',','.','/',
0,0,0,'\0'
};





/*Local Variable*/
// char* key_buf[KEY_BUF_SIZE] = {0};

/* scancode set, divided into 4 sets*/
// CP1 : we only use a limited set 1
// No cap no shift


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

    if( (i%2 !=1) || pre == value){
        putc(value);
    }
    send_eoi(KEYBOARD_IRQ_NUM);
    i++;
    pre = value;


 
    return;
};

