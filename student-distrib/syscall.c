#include "syscall.h"
#include "lib.h"
#include "types.h"
#include "devices/RTC.h"
#include "devices/terminal.h"
// no need linkage include
// maybe no need keyboard
//jump tables for open, close, read, write


// file_ops_t std_operations = {term_open, term_close, term_read, term_write};
// file_ops_t dir_operations = {dir_open, dir_close, dir_read, dir_write};
// file_ops_t file_operations = {file_open, file_close, file_read, file_write};
// file_ops_t rtc_operations = {rtc_open, rtc_close, rtc_read, rtc_write};

pcb_t pcb_1;

void pcb_init ()) {
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
        pcb_1.entry[i] = default_fd_entry;
    }
    
}

/* 
 * sys_open
 *  DESCRIPTION: Find the file in the file system and assign an unused file descriptor
 *                  File descriptors need to be set up according to the file type
 *  INPUTS: filename -- interrupt request id, 0 - 15
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: Enable (unmask) the specified IRQ
 */
int32_t sys_open (const uint8_t* filename) {
    
    printf ("sys_open called\n");
    int i;
    fd_entry_t new_fd_entry;
    //new_fd_entry.fot_ptr = 0;
    new_fd_entry.inode_num = 0;
    new_fd_entry.file_pos = 0;
    new_fd_entry.flag = 0;
    if (file_open (filename, &new_fd_entry) == 0) {
        //TODO: add the fd_entry to pcb
        for (i = 0; i < 6; i++) {
            if (pcb_1.fd_entry[i].flag == 0) {
                pcb_1.fd_entry[i] = new_fd_entry;
                break;
            }
            // if no fd left,  what to do ?
        }
        printf (" %s found, inode number is %u\n", filename, pcb_1.entry[i].inode_num);
        return 0;
    }
    return -1;
}
/* 
 * enable_irq
 *  DESCRIPTION: enable the specific port on i8259,
 *       accept the interrup specified by irq_num
 *  INPUTS: irq_num -- interrupt request id, 0 - 15
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: Enable (unmask) the specified IRQ
 */
int32_t sys_close (const uint8_t* filename) {
    printf ("sys_close called\n");
    return 0;

}

int32_t sys_read (void) {
    printf ("sys_read called\n");

    // check a lot of || , and simply return 


    return 0;
}

int32_t sys_write (void) {
    printf ("sys_write called\n");
    // connect to the terminal and filesystem?
    return 0;
}
