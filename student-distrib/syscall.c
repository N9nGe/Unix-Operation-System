#include "syscall.h"

/*Local constant*/
#define FAIL        -1
#define SUCCESS     0
#define FD_MIN      2
#define FD_MAX      7

#define USER_SPACE_START 0x8000000
#define USER_SPACE_END   0x8400000

#define RTC_INDEX   0
#define DIR_INDEX   1
#define FILE_INDEX  2

/*file operation table pointer */
/*The following are a series of device-speific
  setup helper variable */
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

file_op_t dir_op = {
    .open = dir_open,
    .read = dir_read,
    .write = dir_write,
    .close = dir_close,
};

// stdin file operation (using terminal stuff)
file_op_t terminal_stdin = {
    .open = terminal_open,
    .read = terminal_read,
    .write = NULL,
    .close = NULL,
};

// stdout file operation (using terminal stuff)
file_op_t terminal_stdout = {
    .open = terminal_open,
    .read = NULL,
    .write = terminal_write,
    .close = NULL,
};


pcb_t pcb_1; // modify to the 
// file_op_t fop_table[2];  // Whether to use 
int test_fd;


// TODO:  delete it 
int32_t sys_execute(void){
    return 0;
};

int32_t sys_halt(void){
    return 0;
};

void pcb_init (){
    fd_entry_init(pcb_1.fd_entry);
}

/* fd_entry_init(fd_entry_t* fd_entry)
 * Description: initilize the given fd array and fill the stdin, stdout stuff
 * Inputs: fd_entry_t* fd_entry -- the fd array need to be initilize
 * Return Value: 0 - success; -1 - fail
 * Function: initilize the given fd array and fill the stdin, stdout stuff
 */
int32_t fd_entry_init(fd_entry_t* fd_entry) {
    if (fd_entry == NULL) {
        return -1;
    }
    uint8_t i;
    for (i= 0; i < 8; i++) {
        fd_entry[i].file_pos = 0;
        fd_entry[i].flag = 0;
        fd_entry[i].fot_ptr = NULL;
        fd_entry[i].inode_num = 0;
        fd_entry[i].filetype = 0;
    }
    fd_entry[0].flag = 1;
    fd_entry[0].fot_ptr = &terminal_stdin;
    fd_entry[1].flag = 1;
    fd_entry[1].fot_ptr = &terminal_stdout;

    return 0;
}


int32_t find_next_fd() {
    int i;
    for (i = 2; i < 8; i++) {
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
 *  INPUTS: filename -- string, zero-pad to 32B length 
 *  OUTPUTS: none
 *  RETURN VALUE: 
 *      FAIL    -- -1 
 *      SUCCESS -- 0
 *  SIDE EFFECTS: none
 */
int32_t sys_open (const uint8_t* filename) {
    
    printf ("sys_open called\n");
    // Boundary check: making sure file is within the user space, end - 32 is for 32B length 
    if(filename == NULL ){
        printf("1 failed to open %s\n",filename);
        return FAIL;
    }
    // BUG: Always null filename

    int i;  // Loop index
    int fd = -1; // file descriptor
    /* select bit */
    // int file_type; // 0 for RTC, 1 for dir, 2 for regular file (including terminal)
    fd_entry_t new_fd_entry;
    //new_fd_entry.fot_ptr = 0;
    new_fd_entry.inode_num = 0;
    new_fd_entry.file_pos = 0;
    new_fd_entry.flag = 1;
    fd = find_next_fd();
    // int print_fd  = fd+2;
    //printf("current fd is %d || ",print_fd);
    /*test function used for read and close*/  
    if (fd <2) return FAIL;

    // If failed to open the file, quit it
    if (file_open (filename, &new_fd_entry) != 0) {
        printf("2 failed to open %s\n",filename);
        return FAIL;
    } else {
        if (fd > 0) {
            //printf("%u x\n", new_fd_entry.filetype);
            switch (new_fd_entry.filetype) {
                case 0:
                    new_fd_entry.fot_ptr = (&rtc_op);
                    printf("rtc\n");
                    break;
                case 1:
                    new_fd_entry.fot_ptr = (&dir_op);
                    printf("dir\n");
                    break;
                case 2:
                    new_fd_entry.fot_ptr = (&file_op);
                    printf("file\n");
                    break;
            }
            pcb_1.fd_entry[fd] = new_fd_entry;
        } 
        // TODO directory case? file type decision
    }

    
    //printf (" %s found, inode number is %u\n", filename, pcb_1.fd_entry[fd].inode_num); // debug usage
    //printf (" %u\n", fd); // debug usage

    // file_type = new_fd_entry.filetype;
    // new_fd_entry.fot_ptr = fop_table[file_type];
    // pcb_1.fd_entry[fd] = new_fd_entry;
    

    return 0;
    
}

/* 
 * sys_close
 *  DESCRIPTION: clear the file descripter passed by the argument
 *  INPUTS: int32_t fd -- file descriptor, range from 2 - 7
 *  OUTPUTS: none
 *  RETURN VALUE: 0 if success. Else, FAIL -1
 *  SIDE EFFECTS: Change fd_entry stored in pcb array
 */
int32_t sys_close (int32_t fd) {
    printf ("sys_close called\n");
    if (fd < 2 || fd > 7) {
        return -1;
    }
    int32_t idx = fd;
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
 *  SIDE EFFECTS: none
 */
int32_t sys_read (int32_t fd, uint8_t* buf, int32_t nbytes){
    printf ("sys_read called\n");
    // if((fd < FD_MIN || fd > FD_MAX ) ||
    //    (buf == NULL || nbytes < 0  ) ||
    //    ((int)buf < USER_SPACE_START  || (int)buf + nbytes > USER_SPACE_END ) ||
    //    (pcb_1.fd_entry[fd].flag == 0 ) ||
    //    (pcb_1.fd_entry[fd].fot_ptr->read == NULL)
    // ){
    //     printf("failed to read fd: %d\n",fd);
    //     return FAIL;
    // }
    printf("Reading fd: %d\n",fd);
  /*Function code is one line the return value */
    int32_t ret = (*(pcb_1.fd_entry[fd].fot_ptr->read))(fd, buf, nbytes); 
    return ret;
}
/* 
 * sys_write
 *  DESCRIPTION: Use file operations jump table to 
 *  call the corresponding write function

 *  INPUTS: 
        fd  -- file descriptor number
        buf -- b 
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS:none
 */
int32_t sys_write (int32_t fd, const uint8_t* buf, int32_t nbytes){
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
    int32_t ret = (*(pcb_1.fd_entry[fd].fot_ptr->write))(fd, buf, nbytes); 
    return ret;
}



