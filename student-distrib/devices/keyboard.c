/* keyboard.c - Functions to interact with the Keyboard interrupt device
 * 
 * Tony  1  10.14&15.2022 CP1 keyboard
 * Tony  2  10.16.2022    reconstuct the keyboard.c
 * Tony  3  10.20.2022    Add Capslock and Shift
 * Tony  4  10.22.2022    Add Backspace
 */
#include "../lib.h"
#include "../types.h"
#include "../i8259.h"
#include "../x86_desc.h"
#include "keyboard.h"
#include "terminal.h"
/* File Scope Variable*/

/* Set to avoid release repetition*/
int i = 0;            // caps_lock counter 
int ctrl_buf = 0;     // Ctrl buf, used to clear up the screen
int shift_buf = 0;    // shift buf, when it is pressed, cap & symbols
int caps_lock = 0;    // Capitalize the charcter
int alt_buf = 0;      // Alt buf, do nothing now

// CP5: the bit to decide which terminal
int last_terminal = 1; 
uint8_t keyboard_buf_arr[3][KEY_BUF_SIZE];
int keybuf_count_arr[3] = {0,0,0};

uint8_t keyboard_buf[KEY_BUF_SIZE];
int     keybuf_count = 0;
// TOOD How to change this
volatile int  kb_flag = 0;                // flag used to open the terminal read 

/*The CP2 edition scancode list*/
// CP1 : we only use a limited set 1
// we improved the layout pattern of scancode, to make it more easily modified
unsigned char scancode[MAX_SCAN_SIZE][2] = 
{   {0x0, 0x0}, // 0x00 not use
    {0x0, 0x0}, // 0x01 esc
    /* 0x02 - 0x0e: "1" -> backspace */
    {'1', '!'}, 
    {'2', '@'},
    {'3', '#'}, 
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'}, 
    {'8', '*'},
    {'9', '('}, 
    {'0', ')'},
    {'-', '_'}, 
    {'=', '+'},
    {'\b', '\b'}, //backspace
/* 0x0f - 0x1b: tab -> "}" */
    {'\t', '\t'}, // tab
    {'q', 'Q'}, 
    {'w', 'W'},
    {'e', 'E'}, 
    {'r', 'R'},
    {'t', 'T'}, 
    {'y', 'Y'},
    {'u', 'U'}, 
    {'i', 'I'},
    {'o', 'O'}, 
    {'p', 'P'},
    {'[', '{'}, 
    {']', '}'},
/* 0x1c - 0x28: enter -> "'"*/
    {'\n', '\n'}, // enter
    {0x0, 0x0}, // Left Ctrl
    {'a', 'A'},
    {'s', 'S'},
    {'d', 'D'}, 
    {'f', 'F'},
    {'g', 'G'}, 
    {'h', 'H'},
    {'j', 'J'}, 
    {'k', 'K'},
    {'l', 'L'}, 
    {';', ':'},
    {SINGLE_QUATE, DOUBLE_QUATE},
/* 0x29 - 0x39 : "`" -> SPACE*/
    {'`', '~'}, // the mapping is so confusing for this 
    {0x0, 0x0}, // Left Shift
    {'\\', '|'},
    {'z', 'Z'}, 
    {'x', 'X'},
    {'c', 'C'}, 
    {'v', 'V'},
    {'b', 'B'}, 
    {'n', 'N'},
    {'m', 'M'}, 
    {',', '<'},
    {'.', '>'}, 
    {'/', '?'},
    {0x0, 0x0}, // Right Shift
    {0x0, 0x0}, // Left Control
    {0x0, 0x0}, // left alt?  I wonder where is the Fn
    {' ', ' '}, // SPACE
};




/* 
 * keyboard_init
 *  DESCRIPTION: Initialize keyboard input device 
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none
 */
void keyboard_init(void){
    // printf("initialize keyboard...");
    enable_irq( KEYBOARD_IRQ_NUM );
    memset(keyboard_buf,NULL,sizeof(keyboard_buf)); // CP4 add
    return;
};
/* 
 * keyboard_interrupt_handler
 *  DESCRIPTION: the interrupt handler to deal with keyboard input 
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: set kb_flag on when \n is entered
 */
void keyboard_interrupt_handler(){
    cli(); // Clear all the interrupt first
    send_eoi(KEYBOARD_IRQ_NUM); // end present interrupt
    // NOTICE: it must be here! 
    unsigned int key;
    unsigned int value;
    // Get key interrupt from the pic port 0x60
    key = inb(KEYBOARD_PORT) & 0xff; // & with 0b1111 1111 to control as char
    value = scancode[key][0];        // default as smaller
    if (function_key_handle(key) == 1){
        sti();
        return;
    }
    if ( (keybuf_count ==  (KEY_BUF_SIZE -1)) ){ // leave the last bit for \n or \b
        if( value == '\n' || value == '\b'){
            // printf("\nfirst count is %d\n",keybuf_count); // TEST
        }else{
            sti();
            return; // Reject further input
        }
    }
    // Ignore the key out of the scope of scan size
    if (key > INITIAL_KEY && key <= MAX_SCAN_SIZE){
        if( shift_buf == 1){// decide the scancode
            value = scancode[key][1];
        }
            // Clear the screen when necessary
                if ( value == '\n' ){
                    // memset(keyboard_buf,NULL,sizeof(keyboard_buf)); // TODO: MOVE TO TERMINA
                    terminal[running_term].count = keybuf_count +1;
                    if(terminal[running_term].count == KEY_BUF_SIZE - 2) {
                        keyboard_buf[keybuf_count +1] = '\n';
                    } else {
                        keyboard_buf[keybuf_count] = '\n';
                    }
                    keybuf_count = 0;
                    kb_flag = 1;            // interrupt the terminal 
                    putc_advanced(value);
                    // printf("\nsecond count is %d\n",keybuf_count); // TEST
                    sti();
                    return;
                }
                if (ctrl_buf == 1 && (value == 'l' || value == 'L')){
                    clear();
                    memset(keyboard_buf,NULL,sizeof(keyboard_buf));
                    
                    keybuf_count = 0;
                    // printf("Cleared the screen: "); // TEST
                    sti();
                    return;
                }
                    if (ctrl_buf == 1){
                        switch (value)
                        {
                        case 49: // replace this key to f1
                            running_term = 1;
                            break;
                        case 50: // replace this key to f2
                            running_term = 2;
                            break;
                        case 51: // replace this key to f3
                            running_term = 3;
                            break;
                        default:
                            // running term = 0 means that invalid terminal number
                            running_term = 0;
                            break;
                        }
                        // if the terminal number is invalid, ignore the command
                        if (running_term == 0) {
                            sti();
                            return;
                        }
                        if(running_term != last_terminal){
                            //printf("current terminal: %u\n", running_term);
                            switch_screen(last_terminal, running_term);
                            memcpy(keyboard_buf_arr[last_terminal-1], keyboard_buf, KEY_BUF_SIZE);
                            memcpy(keyboard_buf, keyboard_buf_arr[running_term-1], KEY_BUF_SIZE);
                            keybuf_count_arr[last_terminal-1] = keybuf_count;
                            keybuf_count = keybuf_count_arr[running_term-1];

                            last_terminal = running_term;
                            sti();
                            return;
                        } 
                    }

                if( value ==  '\b' && keybuf_count >= 0){
                    if (keybuf_count == 0){
                        sti();
                        return;
                    }
                    backspace_handler();
                    sti();
                    return;
                }
                if( value == '\t'){ // Tab output, here I use the easiest way
                    putc_advanced(' ');
                    putc_advanced(' ');
                    putc_advanced(' ');
                    putc_advanced(' ');
                    // keybuf_count++;
                    keyboard_buf[keybuf_count] = '\t'; 
                    keybuf_count++;
                    sti();
                    return;
                }
                
                if ((value >= 'a' && value <= 'z') && caps_lock == 1){
                    value = scancode[key][1]; // check if caps_lock is on
                }
                
                    putc_advanced(value);
                    keyboard_buf[keybuf_count] = value;
                    keybuf_count++;
    }
    
    sti();
    return;
}

/* 
 * backspace_handler
 *  DESCRIPTION: call backspace to delete the specific char loaded in the kb_buf
 *  INPUTS:  none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: delete a char on the screen
 */
void backspace_handler(){
    keybuf_count--;
    if (keyboard_buf[keybuf_count] == '\t'){
        backspace();
        backspace();
        backspace();
        backspace();
    }else{
        backspace();
    }
}


/* 
 * function_key_handle
 *  DESCRIPTION: set the function key buf according to different interrupt
 *  INPUTS: key -- the input key value
 *  OUTPUTS: none
 *  RETURN VALUE: 
 *      VALID_RET   -- 0, if there is no function key pressed
 *      INVALID_RET -- 1, if there is function key pressed
 *  SIDE EFFECTS: modify the function key buf
 */
int function_key_handle(unsigned int key){
    int ret = VALID_RET; 
    switch (key)
    {// NOTICE: These function keys are left for cp2
    case LEFT_SHIFT_PRESSED:
        shift_buf = 1;
        ret = INVALID_RET;
        break;
    case LEFT_SHIFT_RELEASED:
        shift_buf = 0;
        ret = INVALID_RET;
        break;

    case RIGHT_SHIFT_PRESSED:
        shift_buf = 1;
        ret = INVALID_RET;
        break;
    case RIGHT_SHIFT_RELEASED:
        shift_buf = 0;
        ret = INVALID_RET;
        break;
    case CTRL_PRESSED:
        ret = INVALID_RET;
        ctrl_buf = 1;
        break;
    case CTRL_RELEASED:
        ret = INVALID_RET;
        ctrl_buf = 0;
        break;    
    case CAPSLOCK_PRESSED:
        ret = INVALID_RET;
        i++;
        caps_lock = 1;
        break;
    case CAPSLOCK_RELEASED:
        caps_lock = (i%2); // Use module to decide whether the lock should be changed
        ret = INVALID_RET;

        break;
    case ALT_PRESSED:
        // printf(" alt is pressed "); // TEST alt press
        alt_buf = 1;
        ret = INVALID_RET;
        break;
    case ALT_RELEASED:
        // printf(" alt is released "); // TEST alt release
        alt_buf = 0;
        ret = INVALID_RET;
        break;
    default:
    
        break;
    }
   if(i > MAX_INPUT_COUNT){
    reset_keyboard_buffer();
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
 *  SIDE EFFECTS: reset two global variable 
 */
void reset_keyboard_buffer(){
    i = 0;
    ctrl_buf = 0;
    shift_buf = 0;
    caps_lock = 0;
    alt_buf = 0;
}

