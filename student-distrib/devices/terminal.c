/* terminal.c - Functions to interact with the terminal 
 * 
 * Tony  1  10.20.2022  -- initialize terminal
 * Tony  2  10.22.2022  -- finish basic structure, init, open and close
 * Gabriel 1 10.23.2022 -- finish read 
 * Tony  3  10.23.2022  -- finish write and test function 
 * 
 */
#include"../lib.h"
#include"../types.h"
#include"keyboard.h"
#include"terminal.h"

terminal_t main_terminal;
/* 
 * terminal_init
 *  DESCRIPTION: Initialize terminal display
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none
 */
void terminal_init(){
    // printf("initialize terminal...");
    main_terminal.id = 0;
    main_terminal.index = 0;
    memset(main_terminal.buf, NULL, sizeof(main_terminal.buf));
    return;
};

void terminal_reset(terminal_t terminal){
    int i; // loop variable
    terminal.id = 0;
    terminal.index = 0;
    for(i = 0; i < TERMINAL_BUF_SIZE ; i++){
        terminal.buf[i] = NULL;
    }
}

/* 
 *  int32_t terminal_read(int32_t fd, void* buf, uint32_t nbytes)
 *  DESCRIPTION: terminal buffer read contents from kb_buf
 *  INPUTS: 
 *     fd     -- file descripter buffer 
 *     buf    -- the terminal buffer to read 
 *     nbytes -- the bytes numbers need to be read
 *  RETURN VALUE:
 *     number of bytes successfully copied
 */
int32_t terminal_read(int32_t fd, void* buf, uint32_t nbytes){
    //TODO: what the relationship between nbytes and kb_count?
    // check whether the fd is valid
    if (fd < 2 || fd > 7){
        printf("Fd must be in [2,7]\n ");
        return -1;
    }
    // check whether the buffer is invalid
    if(buf == NULL){
        return -1;
    }
    // the variable used for return
    int32_t copy_byte;
    // loop index for copy
    int32_t index;
    // set the flag off and wait for the enter pressed
    kb_flag = 0;
    while (kb_flag == 0);

    // copy nbytes from the keyboard 
    for (index = 0; index < nbytes; index++) {
        // the buf still have character
        if (index < keybuf_count) {
            *(uint8_t*)buf = keyboard_buf[index];
        } else {
            //TODO: if we do not have the enough staff to copy, what should we fill
            // Could we break directly break the copy process?
            // the buffer do not have enough stuff, just fill null?
            *(uint8_t*)buf = 0;
        }
    }
    // choose the return n bytes  
    if (nbytes <= keybuf_count) {
        copy_byte = nbytes;
    } else {
        copy_byte = keybuf_count;
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
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    // check whether the fd is valid
    if (fd < 2 || fd > 7){
        printf("Fd must be in [2,7]\n ");
        return -1;
    }
    // check whether the buffer is invalid
    if(buf == NULL){
        return -1;
    }
    // the variable used for return
    int32_t copy_byte;
    // loop index for copy
    int32_t index;
    // Output charcter
    int8_t  c;
    // set the flag off and wait for the enter pressed
    kb_flag = 0;
    while (kb_flag == 0);
    const char* char_buf = buf; 
    // copy nbytes from the buffer to the terminal 
    for (index = 0; index < nbytes; index++) {
        // the buf still have character
        if (index < keybuf_count-1) { // last \n
            c = char_buf[index];
            putc_advanced(c);
        }
    }
    // choose the return n bytes  
    if (nbytes <= keybuf_count) {
        copy_byte = nbytes;
    } else {
        copy_byte = keybuf_count;
    }

    return copy_byte;// return number of bytes successfully copied
}

/* 
 * terminal_open
 *  DESCRIPTION: open the file
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: 0 as success
 *  SIDE EFFECTS: none
 */
int32_t terminal_open(const uint8_t* filename){
    if ( filename == NULL){
        printf("Can't read null file\n ");
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



