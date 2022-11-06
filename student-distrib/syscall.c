#include "syscall.h"
#include "lib.h"
#include "types.h"
#include "devices/RTC.h"
#include "devices/terminal.h"
// no need linkage include
//jump tables for open, close, read, write


// file_ops_t std_operations = {term_open, term_close, term_read, term_write};
// file_ops_t dir_operations = {dir_open, dir_close, dir_read, dir_write};
// file_ops_t file_operations = {file_open, file_close, file_read, file_write};
// file_ops_t rtc_operations = {rtc_open, rtc_close, rtc_read, rtc_write};

pcb_t pcb_1;

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
        for (i = 0; i < 6; i++) {
            if (pcb_1.fd_entry[i].flag == 0) {
                if (strncmp_unsigned(filename, "rtc", 32) == 0) {
                    new_fd_entry.fot_ptr = set_rtc_fop;
                } else if (strncmp_unsigned(filename, "rtc", 32) == 0) {

                }
                pcb_1.fd_entry[i] = new_fd_entry;
                
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
 *  DESCRIPTION: enable the specific port on i8259,
 *       accept the interrup specified by irq_num
 *  INPUTS: irq_num -- interrupt request id, 0 - 15
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: Enable (unmask) the specified IRQ
 */
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes){
    printf ("sys_read called\n");

    // check a lot of || , and simply return 


    return 0;
}
/* 
 * sys_write
 *  DESCRIPTION: enable the specific port on i8259,
 *       accept the interrup specified by irq_num
 *  INPUTS: irq_num -- interrupt request id, 0 - 15
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: Enable (unmask) the specified IRQ
 */
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
    printf ("sys_write called\n");
    // connect to the terminal and filesystem?
    return 0;
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
j
file_op_t set_terminal_fop(){
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
}

