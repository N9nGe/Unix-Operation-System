/* terminal.c - Functions to interact with the terminal 
 * 
 * Tony  1  10.20.2022  -- initialize terminal
 * Tony  2  10.22.2022  -- finish basic structure, init, open and close
 * Gabriel 1 10.23.2022 -- finish read 
 * Tony  3  10.23.2022  -- finish write and test function 
 * Tony  4  11.26.2022  -- start working on mult_terminal
 */
#include"../lib.h"
#include"../types.h"
#include"keyboard.h"
#include"terminal.h"
//CP5: I allocate 4 terminal array
terminal_t terminal[4];
int running_term = 1;

/* 
 * terminal_init
 *  DESCRIPTION: Initialize terminal display
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none
 */
void terminal_init(){
    // TODO: how to execute 3 shell at the very begining?
    int i; // loop index
    for ( i = 0; i < 4; i++)
    {
        terminal[i].id = i;
        terminal[i].index = 0;
        terminal[i].count = 0;
        memset(terminal[i].buf, NULL,sizeof(terminal[i].buf));
    }
    return;
};
/* Helper function
 * - reset current terminal and relevent pcb
 */
void terminal_reset(terminal_t terminal_tmp){
    // TODO: reset or halt the running
    terminal_tmp.id = 0;
    terminal_tmp.index = 0;
    terminal_tmp.count = 0;
    memset(terminal_tmp.buf, NULL, sizeof(terminal_tmp.buf));
}

/* 
 *  int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 *  DESCRIPTION: terminal buffer read contents from kb_buf
 *  the buf should be terminated by \n or as much as fits in buf
 *  INPUTS: 
 *     fd     -- file descripter buffer 
 *     buf    -- the terminal buffer to read 
 *     nbytes -- the bytes numbers need to be read
 *  RETURN VALUE:
 *     number of bytes successfully copied
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    // the variable used for return
    int32_t copy_byte;
    // loop index for copy
    int32_t index;
    // set the flag off and wait for the [enter] pressed
    kb_flag = 0;
    while (kb_flag == 0); // lock the terminal until the keyboard flag is set 1
    memset(buf,NULL,sizeof(buf));
    // copy nbytes from the keyboard buffer
    for (index = 0; index < nbytes; index++) {
        // Only copy the required part 
        if (index <= terminal[running_term].count) {
            ((uint8_t*)buf)[index] = keyboard_buf[index];
        } else {
            ((uint8_t*)buf)[index] = 0;
        }
    }
    memset(keyboard_buf,NULL,sizeof(keyboard_buf));
    // choose the return n bytes  
    if (nbytes <= terminal[running_term].count) {
        copy_byte = nbytes;
    } else {
        copy_byte = terminal[running_term].count;
    }

    return copy_byte;// return number of bytes successfully copied
}

/* 
 * terminal_write
 *  DESCRIPTION: output the content in the buffer into terminal
 *  INPUTS: 
 *      fd      -- file descriptor 
 *      buf     -- buffer used to 
 *      nbytes  --
 *  OUTPUTS: none
 *  RETURN VALUE: 
 *      0  -- success
 *      -1 -- fail
 *  SIDE EFFECTS: none
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    // check whether the buffer is invalid
    if(buf == NULL){
        return -1;
    }
    // the variable used for return
    int32_t copy_byte;
    // loop index for copy
    int32_t index;
    // Output charcter
    uint8_t  c;
    int32_t buf_length = strlen(buf);
    const uint8_t* char_buf = buf; 
    // copy nbytes from the buffer to the terminal 
    for (index = 0; index < nbytes; index++) {
        // the buf still have character
            c = char_buf[index];
            putc_advanced_multi(c, running_term);
    }
    
    // memset(buf,NULL,sizeof(buf));
    // choose the return n bytes  
    if (nbytes <= buf_length) {
        copy_byte = nbytes;
    } else {
        copy_byte = buf_length;
    }
    return copy_byte;// return number of bytes successfully copied
}

/* 
 * terminal_open
 *  DESCRIPTION: open the file
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: 0 as success
 *                -1 as FAILs
 *  SIDE EFFECTS: none
 */
int32_t terminal_open(const uint8_t* filename){
    if ( filename == NULL){
        printf("Can't read null file in ter\n ");
        return -1;
    }else{
        return 0;
    }
}// CP2: we defaultly set it as 0 -- success 

/* 
 * terminal_close
 *  DESCRIPTION: close the file
 *  INPUTS: fd -- file descriptor, corrsponding specificed file
 *  OUTPUTS: none
 *  RETURN VALUE: 0 as success
 *  SIDE EFFECTS: none
 */
int32_t terminal_close(int32_t fd){
    if(fd >=2 && fd <= 8){
        return 0;
    }else{
        printf("Fd must be in [2,8]\n ");
        return -1;
    }
}// CP2: we defaultly set it as 0 -- success 



