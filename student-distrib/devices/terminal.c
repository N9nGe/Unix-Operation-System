/* terminal.c - Functions to interact with the terminal 
 * 
 * Tony  1  10.20.2022  -- initialize terminal
 * Tony  2  10.22.2022  -- 
 * 
 */
#include"../lib.h"
#include"../types.h"
#include"keyboard.h"
#include"terminal.h"

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
    static terminal_t SCREEN;
    SCREEN.id = 0;
    SCREEN.index = 0;
    SCREEN.keyboard_buf[0] = '0'; // for testing
    return;
};

void terminal_reset(terminal_t terminal){
    int i; // loop variable
    terminal.id = 0;
    terminal.index = 0;
    for(i = 0; i < KEY_BUF_SIZE ; i++){
        terminal.keyboard_buf[i] = NULL;
    }


}

/* 
 * terminal_read
 *  DESCRIPTION: Initialize terminal display
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none
 */
int32_t terminal_read(int32_t fd, void* buf, uint32_t nbytes){
    // 	if(nbytes!=LINE_BUFFER_SIZE || buf==NULL){
//         return FAIL;
//     }
// 	strncpy((int8_t*) buf, line_buffer, buffer_index);
// 	return buffer_index;
//
    //printf("terminal_read called, buffer address：%x",&buf);

    terminals[scheduled_index].terminal_read_flag = 1;
    terminals[scheduled_index].stdin_enable = 0;
    if(buf == NULL){
        return FAIL;
    }
    return copy_buffer(buf); // need to build a bufer?
}

/* 
 * terminal_write
 *  DESCRIPTION: Initialize terminal display
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    int32_t temp_count; // TODO
    cli();
    if(buf == NULL){
        return FAIL;
    }
    const char* char_buf = buf;  
    for(temp_count = 0; temp_count < nbytes; temp_count++){
        putc(char_buf[temp_count]);
    }
    sti();
    /* return # of bytes written */
    return temp_count;
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
    return 0;
}// CP2: we defaultly set it as 0 -- success 

/* 
 * terminal_close
 *  DESCRIPTION: close the file
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: 0 as success
 *  SIDE EFFECTS: none
 */
int32_t terminal_close(int32_t fd){
    return 0;
}// CP2: we defaultly set it as 0 -- success 



