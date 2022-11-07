#include "syscall.h"

/*Local constant*/
#define FAIL        -1
#define SUCCESS     0
#define FD_MIN      2
#define FD_MAX      7

#define USER_SPACE_START 0x8000000
#define USER_SPACE_END   0x8400000

static uint32_t task_counter = 0;
pcb_t * current_pcb_pointer;

/*file operation table pointer */
/*The following are a series of device-speific
  setup helper variable */
// RTC ORWC
file_op_t rtc_op = {
    .open = rtc_open,
    .read = rtc_read,
    .write = rtc_write,
    .close = rtc_close,
};
// Regular file ORWC
file_op_t file_op = {
    .open = file_open,
    .read = file_read,
    .write = file_write,
    .close = file_close,
};
// Directory File ORWC
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


/* pcb_initilize()
 * Description: initilize the given pcb and set it active
 * Inputs: none
 * Return Value: initialized pcb
 * Function: initilize the given pcb and set it active, also initialize the fd array
 */
pcb_t* pcb_initilize() {
    pcb_t* pcb = find_pcb();
    pcb->active = 1;
    fd_entry_init(pcb->fd_entry);
    return pcb;
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

/* find_pcb()
 * Description: get the current pcb pointer
 * Inputs: task_pointer, a global variable used to linearly accumulate the pcb
 * Return Value: current pcb pointer
 * Function: get the current pcb by the task counter
 */
pcb_t* find_pcb() {
    return ((pcb_t*) (KERNEL_BOTTOM - PROCESS_SIZE * (task_counter)));
}


int32_t find_next_fd() {
    pcb_t * pcb_1;
    pcb_1 = find_pcb();
    int i;
    for (i = 2; i < 8; i++) {
        if (pcb_1 -> fd_entry[i].flag == 0) {
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
 *      SUCCESS -- fd
 *  SIDE EFFECTS: none
 */
int32_t sys_open (const uint8_t* filename) {
    pcb_t * pcb_1;
    pcb_1 = find_pcb();
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

    // fd_entry_t new_fd_entry;
    // new_fd_entry.inode_num = 0;
    // new_fd_entry.file_pos = 0;
    // new_fd_entry.flag = 1;
    fd = find_next_fd();
    // pcb_1->fd_entry->inode_num = 0;

    // printf("current fd is %d || ",fd);
    /*test function used for read and close*/  
    if (fd < 2 || fd > 7) return FAIL;

    // If failed to open the file, quit it
    if (file_open (filename) != 0) {
        //printf("2 failed to open %s\n",filename);
        return FAIL;
    } else {
        // dentry_t tmp_dentry;
        // if (read_dentry_by_name (filename, &tmp_dentry) != 0){     // check if read dentry succeeded
        //     return -1;
        // }
        // new_fd_entry.inode_num = tmp_dentry.inode_num;
        // new_fd_entry.filetype = tmp_dentry.filetype;
        dentry_t temp_dentry;
        if (read_dentry_by_name (filename, &temp_dentry) != 0){     // check if read dentry succeeded
            return -1;
        }
        pcb_1->fd_entry[fd].inode_num = temp_dentry.inode_num;
        pcb_1->fd_entry[fd].file_pos = 0;
        pcb_1->fd_entry[fd].flag = 1;
        pcb_1->fd_entry[fd].filetype =temp_dentry.filetype;

        if (fd >= 2) {
            //printf("%u x\n", new_fd_entry.filetype);
            switch (pcb_1->fd_entry[fd].filetype) {
                case 0:
                    pcb_1->fd_entry[fd].fot_ptr = (&rtc_op);
                    printf("rtc\n");
                    break;
                case 1:
                    pcb_1->fd_entry[fd].fot_ptr = (&dir_op);
                    printf("dir\n");
                    break;
                case 2:
                    pcb_1->fd_entry[fd].fot_ptr = (&file_op);
                    printf("file\n");
                    break;
                default: //TODO
                    pcb_1->fd_entry[fd].fot_ptr = (&file_op);
                    printf("file2\n");
                    break;
            }
            // pcb_1 -> fd_entry[fd] = new_fd_entry;
            printf (" %s found, inode number is %u\n", filename, pcb_1->fd_entry[fd].inode_num); // debug usage

        } 
    }
    return fd;
    
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
    pcb_t * pcb_1;
    pcb_1 = find_pcb();
    //printf ("sys_close called\n");
    if (fd < 2 || fd > 7) {
        return -1;
    }
    if (pcb_1 -> fd_entry[fd].flag == 0) {
        return 0;
    }
    pcb_1 -> fd_entry[fd].inode_num = 0;
    pcb_1 -> fd_entry[fd].file_pos = 0;
    pcb_1 -> fd_entry[fd].flag = 0;
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
    //printf ("sys_read called\n");
    pcb_t * pcb_1;
    pcb_1 = find_pcb();
    // if((fd < FD_MIN || fd > FD_MAX ) ||
    //    (buf == NULL || nbytes < 0  ) ||
    //    ((int)buf < USER_SPACE_START  || (int)buf + nbytes > USER_SPACE_END ) ||
    //    (pcb_1.fd_entry[fd].flag == 0 ) ||
    //    (pcb_1.fd_entry[fd].fot_ptr->read == NULL)
    // ){
    //     printf("failed to read fd: %d\n",fd);
    //     return FAIL;
    // }
    //printf("Reading fd: %d\n",fd);
  /*Function code is one line the return value */
    int32_t ret = (*(pcb_1 -> fd_entry[fd].fot_ptr->read))(fd, buf, nbytes); 
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
    //printf ("sys_write called\n");
    pcb_t * pcb_1;
    pcb_1 = find_pcb();
    // return 0;
    // if((fd < FD_MIN || fd > FD_MAX ) ||
    //    (buf == NULL || nbytes < 0  ) ||
    // //    ((int)buf < USER_SPACE_START || (int)buf + nbytes > USER_SPACE_END ) ||
    //    (pcb_1->fd_entry[fd].flag == 0 ) ||
    //    (pcb_1->fd_entry[fd].fot_ptr -> write == NULL)
    // ){
    //     return FAIL;
    // }

//   /*Function code is one line the return value */
    int32_t ret = (*(pcb_1 -> fd_entry[fd].fot_ptr -> write))(fd, buf, nbytes); 
    return ret;
}



/* execute (const uint8_t* command)
 * Description: system call execute
 * Inputs: const uint8_t* command
 * Return Value: 0 succeff; -1 fail
 * Function: do the execute based on the command
 */
int32_t sys_execute (const uint8_t* command){
    if (command == NULL) {
        return -1;
    }
    // filename buffer used by parse the command
    uint8_t filename[FILENAME_LEN];
    // initialize the filename buffer
    memset(filename, 0, FILENAME_LEN);
    // parse the command(fill the command into the buffer)
    parse_arg(command, filename);

    dentry_t execute_file;
    memset(&execute_file, 0, sizeof(execute_file));
    // check whether file exist
    if (read_dentry_by_name(filename, &execute_file) == -1) {
        return -1;
    }

    // check whether the file is an EXE
    uint8_t execute_code_buf[4];
    read_data(execute_file.inode_num, 0, execute_code_buf, 4);
    if (execute_code_buf[0] != 0x7f || execute_code_buf[1] != 0x45 || 
        execute_code_buf[2] != 0x4c || execute_code_buf[3] != 0x46) {
        return -1;
    }

    cli();
    // paging the new memory
    paging_execute();
    // load the file into USER_PROGRAM_IMAGE_START(virtual memory)
    if (read_data(execute_file.inode_num, 0, (uint8_t*) USER_PROGRAM_IMAGE_START, inode_ptr[execute_file.inode_num].length) == 0) {
        return -1;
    }
    
    // create new pcb
    pcb_t* new_pcb = pcb_initilize();
    current_pcb_pointer = new_pcb;
    // update pid and parent_id
    new_pcb->pid = task_counter;
    new_pcb->parent_id = task_counter - 1;
    
    // save old ebp & esp (from review slides)
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");

    // store ebp && esp in the pcb
    new_pcb->saved_ebp = saved_ebp;
    new_pcb->saved_esp = saved_esp;
    
    // prepare for context switch(from kernel to usermode)
    // USER_DS, ESP, EFLAG, CS, EIP
    uint32_t user_data_segment = USER_DS;
    // get byte 24-28 in EXE
    uint32_t eip = *(uint32_t*)(((uint8_t*) USER_PROGRAM_IMAGE_START) + 24);
    uint32_t user_code_segment = USER_CS;
    uint32_t esp = (USER_PROGRAM_IMAGE_START - 0x48000 + 0x400000 - 4); // -4 because dereference is 4 byte value(avoid page fault)
    
    // TSS
    // no need to change ss0 because kernel using the same kernel stack(initilize at booting)
    // get the current stack address
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (task_counter - 1) - 4);
    sti();

    // context switch && IRET
    asm volatile(
        "xorl %%eax, %%eax;"
        "movl %0, %%eax;"
        "movw %%ax, %%ds;"
        "pushl %0;" 
        "pushl %1;"
        "pushfl;"
        "pushl %2;"
        "pushl %3;"
        "IRET;"
        :
        : "r" (user_data_segment), "r" (esp), "r" (user_code_segment), "r" (eip)
        : "cc", "memory", "eax"
    );

    return 0;
}


/* halt(uint8_t status)
 * Description: system call halt
 * Inputs: uint8_t status
 * Return Value: 0 - succeff; 256 - exception 
 * Function: halt the process and check the causing of the halt
 */
int32_t sys_halt(uint8_t status){
    // get current pcb
    int i;
    uint32_t return_status = (uint32_t) status;
    cli();
    pcb_t* pcb = find_pcb();
    
    // point to the parent kernel stack
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (pcb->parent_id - 1) - 4); 
    
    for (i = 2; i < 8; i++) {
        // need file close
        sys_close(i);
    }
    pcb->fd_entry[0].flag = 0;
    pcb->fd_entry[1].flag = 0;
    pcb->active = 0;
    // restore parent paging
    page_halt(pcb->parent_id);
    
    // jump to execute return
    // there is no program -> need to rerun shell
    sti();
    if (task_counter == 0) {
        sys_execute((uint8_t*)"shell");
    } else {
        asm volatile (
            "movl %0, %%eax;"
            "movl %1, %%ebp;" 
            "movl %2, %%esp;"
            "leave;"
            "ret;"
            :
            : "r" (return_status), "r" (pcb->saved_ebp), "r" (pcb->saved_esp)
            : "eax", "ebp", "esp"  
        );
    } 
    
    return 0;
}

/* parse_arg(uint8_t* command, uint8_t* filename)
 * Description: parse the argument from the commond by the first space
 * Inputs: uint8_t* command: 
 *         uint8_t* filename: 
 * Return Value: none
 * Function: copy the argument into the filename buffer to use later
 */
void parse_arg(const uint8_t* command, uint8_t* filename){
    uint8_t i;
    for (i = 0; i < FILENAME_LEN; i++) {
        // stop at the first space
        if (command[i] != 0x20) {
            filename[i] = command[i];
        } else {
            return;
        }
    }
}

/* paging_execute()
 * Description: allocate the page of the new program
 * Inputs: none
 * Return Value: none
 * Function: allocate the page begin from 8mb in the kernel memory
 */
void paging_execute() {
    // allocate the 4mb page for each process
    uint32_t index = (uint32_t) USER_PROGRAM_IMAGE_START >> PD_SHIFT;
    page_directory[index].pd_mb.present = 1;
    page_directory[index].pd_mb.read_write = 1;
    page_directory[index].pd_mb.user_supervisor = 1;
    page_directory[index].pd_mb.page_size = 1;  // change to 4mb page 
    page_directory[index].pd_mb.base_addr = ((KERNEL_POSITION) + task_counter + 1); // give the address of the process
    task_counter++; // increment the counter
    // flush the TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : : : "eax", "cc"
    );
}

/* page_halt(uint32_t parent_id)
 * Description: unmap the child process, map to the parent
 * Inputs: parent_id - map the memory to the parent memory
 * Return Value: none
 * Function: map to the parent
 */
void page_halt(uint32_t parent_id) {
    // allocate the 4mb page for each process
    uint32_t index = (uint32_t) USER_PROGRAM_IMAGE_START >> PD_SHIFT;
    page_directory[index].pd_mb.present = 1;
    page_directory[index].pd_mb.read_write = 1;
    page_directory[index].pd_mb.user_supervisor = 1;
    page_directory[index].pd_mb.page_size = 1;  // change to 4mb page 
    page_directory[index].pd_mb.base_addr = ((KERNEL_POSITION) + parent_id ); // give the address of the process
    task_counter--; // decrement the counter
    // flush the TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : : : "eax", "cc"
    );
}


