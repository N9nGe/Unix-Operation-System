#include "syscall.h"
#include "lib.h"
#include "types.h"
#include "devices/RTC.h"
#include "devices/terminal.h"
#include "file_system.h"

// no need linkage include
//jump tables for open, close, read, write


// file_ops_t std_operations = {term_open, term_close, term_read, term_write};
// file_ops_t dir_operations = {dir_open, dir_close, dir_read, dir_write};
// file_ops_t file_operations = {file_open, file_close, file_read, file_write};
// file_ops_t rtc_operations = {rtc_open, rtc_close, rtc_read, rtc_write};

pcb_t pcb_1; // modify to the 


file_op_t rtc_op = {
    .open = rtc_open,
    .read = rtc_read,
    .write = rtc_write,
    .close = rtc_close,
};

file_op_t file_op = {
    .open = file_open,
    .read = file_read,
    .write = file_write,
    .close = file_close,
};


void pcb_init (){
    int i;
    pcb_1.pid = 0;         // Current Process id
    pcb_1.parent_id = 0;   // Father process
    pcb_1.fd = 0;
    pcb_1.saved_esp = 0;
    pcb_1.saved_ebp = 0;
    pcb_1.active = 0;
    fd_entry_t default_fd_entry;
    default_fd_entry.inode_num = 0;
    default_fd_entry.file_pos = 0;
    default_fd_entry.flag = 0;
    for (i = 0; i < 6; i++)
    {
        /* code */
        pcb_1.fd_entry[i] = default_fd_entry;
    }
    
}

int32_t find_next_fd() {
    int i;
    for (i = 0; i < 6; i++) {
        if (pcb_1.fd_entry[i].flag == 0) {
            return i;
        }
    }
    return -1;
}

/* 
 * sys_open
 *  DESCRIPTION: Find the file in the file system and assign an unused file descriptor
 *                  File descriptors need to be set up according to the file type
 *  INPUTS: filename -- 32B length 
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: Enable (unmask) the specified IRQ
 */
int32_t sys_open (const uint8_t* filename) {
    
    printf ("sys_open called\n");
    int i;  // Loop index
    fd_entry_t new_fd_entry;
    //new_fd_entry.fot_ptr = 0;
    new_fd_entry.inode_num = 0;
    new_fd_entry.file_pos = 0;
    new_fd_entry.flag = 0;
    if (file_open (filename, &new_fd_entry) == 0) {
        //TODO: add the fd_entry to pcb
        int idx;
        for (i = 0; i < 6; i++) {
            if (pcb_1.fd_entry[i].flag == 0) {
                // set function operation table pointer
                if (find_next_fd() < 0) {
                    return -1;
                }
                idx = find_next_fd();
                if (strncmp_unsigned("rtc", filename, 32) == 0) {
                    new_fd_entry.fot_ptr = (&rtc_op);
                } else {                    
                    new_fd_entry.fot_ptr = (&file_op);
                }
                pcb_1.fd_entry[idx] = new_fd_entry;

                break;
            }
            // if no fd left,  what to do ?
        }
        printf (" %s found, inode number is %u\n", filename, pcb_1.fd_entry[i].inode_num); // debug usage

        return 0;
    }
    return -1;
}

/* 
 * sys_close
 *  DESCRIPTION: clear the file descripter passed by the argument
 *  INPUTS: int32_t fd -- file descriptor, range from 2 - 7
 *  OUTPUTS: none
 *  RETURN VALUE: 0 if success. Else, -1
 *  SIDE EFFECTS: Change fd_entry stored in pcb array
 */
int32_t sys_close (int32_t fd) {
    printf ("sys_close called\n");
    if (fd < 2 || fd > 7) {
        return -1;
    }
    int32_t idx = fd - 2;
    if (pcb_1.fd_entry[idx].flag == 0) {
        return 0;
    }
    pcb_1.fd_entry[idx].inode_num = 0;
    pcb_1.fd_entry[idx].file_pos = 0;
    pcb_1.fd_entry[idx].flag = 0;
    return 0;

}
/* 
 * sys_read
 *  DESCRIPTION: Use file operations jump table to 
 *  call the corresponding read function
 *  INPUTS: fd
 *  OUTPUTS: none
 *  RETURN VALUE: 
 *        - -1 for fail
 *        - returns the number of bytes read.
 *  SIDE EFFECTS: Enable (unmask) the specified IRQ
 */
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes){
    printf ("sys_read called\n");
    // if((fd < FD_MIN || fd > FD_MAX ) ||
    //    (buf == NULL || nbytes < 0  ) ||
    //    ((int)buf < USER_SPACE_START || (int)buf + nbytes > USER_SPACE_END ) ||
    //    (pcb_1.fd_entry[fd].flag == 0 ) ||
    //    (pcb_1.fd_entry[fd].fot_ptr->read == NULL)
    // ){
    //     return FAIL;
    // }
  /*Function code is one line the return value */
    int32_t ret = (*(pcb_1.fd_entry[fd].fot_ptr->read))(fd + 2, buf, nbytes); ;
    return ret;
}
/* 
 * sys_write
 *  DESCRIPTION: Use file operations jump table to 
 *  call the corresponding write function

 *  INPUTS: irq_num -- interrupt request id, 0 - 15
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: Enable (unmask) the specified IRQ
 */
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
    printf ("sys_write called\n");
    return 0;
//     if((fd < FD_MIN || fd > FD_MAX ) ||
//        (buf == NULL || nbytes < 0  ) ||
//        ((int)buf < USER_SPACE_START || (int)buf + nbytes > USER_SPACE_END ) ||
//        (pcb_1.fd_entry[fd].flag == 0 ) ||
//        (pcb_1.fd_entry[fd].file_pos.write == NULL)
//     ){
//         return FAIL;
//     }

//   /*Function code is one line the return value */
//     int32_t ret = (pcb_1.fd_entry[fd].file_pos.write)(fd,buf,nbytes);
//     return ret;
}

/*file operation table pointer */
/*The following are a series of device-speific
  setup helper function */

file_op_t set_rtc_fop(){
    file_op_t new_fop;
    new_fop.close = rtc_close;
    new_fop.open  = rtc_open;
    new_fop.read  = rtc_read;
    new_fop.write = rtc_write;
    return new_fop;
}

/*file_op_t set_terminal_fop(){
    file_op_t new_fop;
    new_fop.close = terminal_close;
    new_fop.open  = terminal_open;
    new_fop.read  = terminal_read;
    new_fop.write = terminal_write;
    return new_fop;
}

file_op_t set_file_fop(){
    file_op_t new_fop;
    new_fop.close = file_close;
    new_fop.open  = file_open;
    new_fop.read  = file_read;
    new_fop.write = file_write;
    return new_fop;
}

file_op_t set_dir_fop(){
        file_op_t new_fop;
    new_fop.close = dir_close;
    new_fop.open  = dir_open;
    new_fop.read  = dir_read;
    new_fop.write = dir_write;
    return new_fop;
}*/

