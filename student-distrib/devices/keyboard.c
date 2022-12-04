/* keyboard.c - Functions to interact with the Keyboard interrupt device
 * 
 * Tony  1  10.14&15.2022 CP1 keyboard
 * Tony  2  10.16.2022    reconstuct the keyboard.c
 * Tony  3  10.20.2022    Add Capslock and Shift
 * Tony  4  10.22.2022    Add Backspace
 * Tony&&Tim  5  11.26.2022    Realized mulit-terminal
 * Gabriel  1  12.2.2022   Debug multi-terminal completely
 */
#include "../lib.h"
#include "../types.h"
#include "../i8259.h"
#include "../x86_desc.h"
#include "keyboard.h"
#include "terminal.h"
/* File Scope Variable*/

/* Function Key status bit --
 * Set to avoid release repetition*/
int caps_lock_counter = 0;            // caps_lock_counter 
int ctrl_buf = 0;     // Ctrl buf, used to clear up the screen
int shift_buf = 0;    // shift buf, when it is pressed, cap & symbols
int caps_lock = 0;    // Capitalize the charcter
int alt_buf = 0;      // Alt buf, do nothing now
volatile int write_flag = 0; // write flag for typing and print

uint8_t keyboard_buf_arr[3][KEY_BUF_SIZE];
int keybuf_count_arr[3] = {0,0,0};

uint8_t keyboard_buf[KEY_BUF_SIZE];
int     keybuf_count = 0;

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
    reset_keyboard_buffer();
    return;
};
/* 
 * keyboard_interrupt_handler
 *  DESCRIPTION: the interrupt handler to deal with keyboard input 
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: set terminal's read_flag on when \n is entered, invoking the terminal_read
 */
void keyboard_interrupt_handler(){
    cli(); // Clear all the interrupt first
    send_eoi(KEYBOARD_IRQ_NUM); // end present interrupt
    // NOTICE: it must be here! 
    unsigned int key;
    unsigned int value;
    // Get key interrupt from the pic port 0x60
    // allow to write
    write_flag = 1;
    key = inb(KEYBOARD_PORT) & 0xff; // & with 0b1111 1111 to control as char
    value = scancode[key][0];        // default as smaller
    if (function_key_handle(key) == 1){
        sti();
        return;
    }
    //CP5: Alt + F1, F2, F3 to switch between the terminals
    int ret = terminal_switch(key);
        if(ret == 1){
            sti();
            return;
        }
    // the display terminal is running pingpong, we can not type
    if (terminal[display_term].pingping_flag == 1) {
        sti();
        return;
    }
    // the display terminal is running fish, we can not type
    if (terminal[display_term].fish_flag == 1) {
        sti();
        return;
    }
    // the display terminal is running grep, we can not type
    if (terminal[display_term].grep_flag == 1) {
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
                    putc_advanced(value); // Why should be there ?
                    terminal[display_term].count = keybuf_count +1;
                    if(terminal[display_term].count == KEY_BUF_SIZE - 2) {
                        keyboard_buf[keybuf_count +1] = '\n';
                    } else {
                        keyboard_buf[keybuf_count] = '\n';
                    }
                    // running_term = display_term;
                    terminal[display_term].read_flag = 1;            // interrupt the terminal 
                    keybuf_count = 0;
                    // printf("\nsecond count is %d\n",keybuf_count); // TEST
                    if(terminal[display_term].read_flag == 0){
                        memset(terminal[display_term].buf,NULL,KEY_BUF_SIZE);
                        terminal[display_term].count = 0;
                    }
                    sti();
                    return;
                }
            /*Ctrl + L to clean the terminal's screen*/
                if (ctrl_buf == 1 && (value == 'l' || value == 'L')){
                    clear();
                    reset_keyboard_buffer();
                    memset(keyboard_buf,NULL,sizeof(keyboard_buf));
                    terminal_reset(terminal[running_term]);
                    keybuf_count = 0;
                    printf("Current Terminal:%d \n",display_term); // TEST
                    printf("391OS> "); // after clean the screen we need to show the terminal name
                    sti();
                    return;
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
    {
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
        caps_lock_counter++;
        caps_lock = 1;
        break;
    case CAPSLOCK_RELEASED:
        caps_lock = (caps_lock_counter%2); // Use module to decide whether the lock should be changed
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
   if(caps_lock_counter > MAX_INPUT_COUNT){
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
    caps_lock_counter = 0;
    ctrl_buf = 0;
    shift_buf = 0;
    caps_lock = 0;
    alt_buf = 0;
    memset(keyboard_buf,NULL,sizeof(keyboard_buf));
    keybuf_count = 0;
}

/* 
 * terminal_switch
 *  DESCRIPTION: a helper function to reset the 
 *  keyboard buffer 
 *  INPUTS: unsigned int value = key currently pressed
 *  OUTPUTS: int success or not
 *  RETURN VALUE: none
 *  SIDE EFFECTS: reset two global variable 
 */
int terminal_switch(unsigned int key){
    int ret = 0;
                 if (alt_buf == 1){ 
                        switch (key)
                        {
                        case F1: // replace this key to f1
                            display_term = 1;
                            ret = 1;
                            break;
                        case F2: // replace this key to f2
                            ret = 1;
                            display_term = 2;
                            break;
                        case F3: // replace this key to f3
                            ret = 1;
                            display_term = 3;
                            break;
                        default:
                            return ret;
                            //break;
                        }
                        if(display_term != last_term){
                            switch_vid_page(last_term, display_term);
                            switch_screen(last_term, display_term);
                            //printf("changinng to terminal %d",display_term); // TEST: current at 3 2 but not 1

                            memcpy(keyboard_buf_arr[last_term-1], keyboard_buf, KEY_BUF_SIZE);
                            memcpy(keyboard_buf, keyboard_buf_arr[display_term-1], KEY_BUF_SIZE);

                            // memcpy(terminal[display_term].buf, keyboard_buf, KEY_BUF_SIZE);

                            keybuf_count_arr[last_term-1] = keybuf_count;
                            keybuf_count = keybuf_count_arr[display_term-1];

                            terminal[last_term].read_flag = 0;

                            last_term = display_term;
                            write_flag = 0;
                            return ret;
                        } 
                    }
    return ret;
}

