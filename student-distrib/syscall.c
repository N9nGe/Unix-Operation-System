#include "syscall.h"

/*Local constant*/
#define SYSCALL_FAIL        -1
#define SYSCALL_SUCCESS     0
#define FD_MIN      2
#define FD_MAX      7

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
    .open  = file_open,
    .read  = file_read,
    .write = file_write,
    .close = file_close,
};
// Directory File ORWC
file_op_t dir_op = {
    .open  = dir_open,
    .read  = dir_read,
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
        return SYSCALL_FAIL;
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

    return SYSCALL_SUCCESS;
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
    for (i = FD_MIN; i < FD_MAX + 1; i++) {
        if (pcb_1 -> fd_entry[i].flag == 0) {
            return i;
        }
    }
    return SYSCALL_FAIL;
}

/* 
 * sys_open
 *  DESCRIPTION: Find the file in the file system and assign an unused file descriptor
 *                  File descriptors need to be set up according to the file type
 *  INPUTS: filename -- string, zero-pad to 32B length 
 *  OUTPUTS: none
 *  RETURN VALUE: 
 *      FAIL    -- -1 
 *      SUCCESS -- fd, the opened file descriptor number
 *  SIDE EFFECTS: none
 */
int32_t sys_open (const uint8_t* filename) {
    pcb_t * pcb_1;
    pcb_1 = find_pcb();
    // Boundary check: making sure file is within the user space, end - 32 is for 32B length 
    if(filename == NULL ){
        printf("1 failed to open %s\n",filename);
        return SYSCALL_FAIL;
    }

    // check filename length is 0
    if(strlen_unsigned(filename) == 0 ){
        printf("Can't open empty filename\n");
        return SYSCALL_FAIL;
    }

    int fd = -1; // file descriptor
    /* select bit */
    fd = find_next_fd();
    /*test function used for read and close*/  
    if (fd < FD_MIN || fd > FD_MAX){
        printf("open failed\n");
        return SYSCALL_FAIL;
    }

    // If failed to open the file, quit it
    if (file_open (filename) != 0) {
        return SYSCALL_FAIL;
    } else {
        dentry_t temp_dentry;
        //printf("%s", filename);

        if (read_dentry_by_name ((uint8_t *) filename, &temp_dentry) != 0){     // check if read dentry succeeded
            //printf("%s", filename);

            return SYSCALL_FAIL;
        }
        pcb_1->fd_entry[fd].inode_num = temp_dentry.inode_num;
        pcb_1->fd_entry[fd].file_pos = 0;
        pcb_1->fd_entry[fd].flag = 1;
        pcb_1->fd_entry[fd].filetype =temp_dentry.filetype;

        if (fd >= FD_MIN) {
            switch (pcb_1->fd_entry[fd].filetype) {
                case 0: // RTC device type
                    pcb_1->fd_entry[fd].fot_ptr = (&rtc_op);
                    break;
                case 1: // directory type
                    pcb_1->fd_entry[fd].fot_ptr = (&dir_op);
                    break;
                case 2: // regular file type
                    pcb_1->fd_entry[fd].fot_ptr = (&file_op);
                    break;
                default:// defaultly set as regular to avoid problem
                    pcb_1->fd_entry[fd].fot_ptr = (&file_op);
                    break;
            }

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
    //get current pcb as pcb_1
    pcb_t * pcb_1;
    pcb_1 = find_pcb();
    // check if fd is closable
    if (fd < FD_MIN || fd > FD_MAX) {
        return SYSCALL_FAIL;
    }
    // reset the designated fd_entry to 0
    if (pcb_1 -> fd_entry[fd].flag == 0) {
        return SYSCALL_FAIL;
    }
    pcb_1 -> fd_entry[fd].inode_num = 0;
    pcb_1 -> fd_entry[fd].fot_ptr = NULL;
    pcb_1 -> fd_entry[fd].filetype = 0;
    pcb_1 -> fd_entry[fd].file_pos = 0;
    pcb_1 -> fd_entry[fd].flag = 0;
    return SYSCALL_SUCCESS;

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
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes){
    //get current pcb as pcb_1
    pcb_t * pcb_1;
    pcb_1 = find_pcb();
    memset(buf, 0, sizeof(buf));
    // check if fd fulfills the requirement
    // check if there's function pointer in the fd
    // check if the nbytes is larger than 0
    if (fd == 1) {
        // printf("1 failed to read fd: %d\n",fd);
        return SYSCALL_FAIL;
    }
    if (fd < 0 || fd > 7 || (buf == NULL || nbytes < 0  ) ||
       (pcb_1->fd_entry[fd].flag == 0 ) ) {
        // printf("2 failed to read fd: %d\n",fd);
        return SYSCALL_FAIL;
    }
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
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
    //get current pcb as pcb_1
    pcb_t * pcb_1;
    pcb_1 = find_pcb();
    // check if fd fulfills the requirement
    // check if there's function pointer in the fd
    // check if the nbytes is larger than 0
    if (fd == 0) {
        printf("failed to write fd: %d\n",fd);
        return SYSCALL_FAIL;
    }
    if (fd < 1 || fd > 7 || (buf == NULL || nbytes < 0  ) ||
       (pcb_1->fd_entry[fd].flag == 0 ) ) {
        // printf("failed to write fd: %d\n",fd);
        return SYSCALL_FAIL;
    }

    /*Function code is one line the return value */
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
        return SYSCALL_FAIL;
    }
    
    // filename buffer used by parse the command
    uint8_t filename[FILENAME_LEN];
    uint8_t tmp_cmd[strlen_unsigned(command)];
    
    // initialize the filename buffer
    memset(filename, 0, FILENAME_LEN);
    memset(tmp_cmd, 0, strlen_unsigned(command));

    strcpy_unsigned(tmp_cmd, command);
    // parse the command(fill the command into the buffer)
    parse_arg(command, filename);

    dentry_t execute_file;
    memset(&execute_file, 0, sizeof(execute_file));
    // check whether file exist
    if (read_dentry_by_name(filename, &execute_file) == SYSCALL_FAIL) {
        return SYSCALL_FAIL;
    }

    // check whether the file is an EXE
    uint8_t execute_code_buf[EXE_CODE_BUF];
    read_data(execute_file.inode_num, 0, execute_code_buf, EXE_CODE_BUF);
    if (execute_code_buf[0] != EXE_MAGIC_1 || execute_code_buf[1] != EXE_MAGIC_2 || 
        execute_code_buf[2] != EXE_MAGIC_3 || execute_code_buf[3] != EXE_MAGIC_4) {
        return SYSCALL_FAIL;
    }

    cli();
    // paging the new memory
    paging_execute();
    // load the file into USER_PROGRAM_IMAGE_START(virtual memory)
    if (read_data(execute_file.inode_num, 0, (uint8_t*) USER_PROGRAM_IMAGE_START, inode_ptr[execute_file.inode_num].length) == 0) {
        return SYSCALL_FAIL;
    }
    
    // create new pcb
    pcb_t* new_pcb = pcb_initilize();
    current_pcb_pointer = new_pcb;
    // update pid and parent_id
    new_pcb->pid = task_counter;
    new_pcb->parent_id = task_counter - 1;
    strcpy_unsigned(new_pcb->cmd, tmp_cmd);
    
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
    uint32_t eip = *(uint32_t*)(((uint8_t*) USER_PROGRAM_IMAGE_START) + TWENTY_FOUR_OFFSET);
    uint32_t user_code_segment = USER_CS;
    uint32_t esp = (USER_PROGRAM_IMAGE_START - USER_PROGRAM_IMAGE_OFFSET + BIG_PAGE_SIZE - AVOID_PAGE_FAULT); // -4 because dereference is 4 byte value(avoid page fault)
    
    // TSS
    // no need to change ss0 because kernel using the same kernel stack(initilize at booting)
    // get the current stack address
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (task_counter - 1) - AVOID_PAGE_FAULT);
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

    return SYSCALL_SUCCESS;
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
    if (exception_flag == 1) {
        return_status = 256;
        exception_flag = 0;
    }
    cli();
    pcb_t* pcb = find_pcb();
    
    // point to the parent kernel stack
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (pcb->parent_id - 1) - AVOID_PAGE_FAULT); 
    
    // close the fd
    for (i = FD_MIN; i < FD_MAX+1; i++) {
        // need file close
        sys_close(i);
    }
    // close stdin and stdout
    pcb->fd_entry[0].flag = 0;
    pcb->fd_entry[0].file_pos = 0;
    pcb->fd_entry[0].inode_num = 0;
    pcb->fd_entry[0].fot_ptr = NULL;
    pcb->fd_entry[1].flag = 0;
    pcb->fd_entry[1].file_pos = 0;
    pcb->fd_entry[1].inode_num = 0;
    pcb->fd_entry[1].fot_ptr = NULL;
    pcb->active = 0;
    memset(pcb->cmd, 0, sizeof(pcb->cmd));
    // restore parent paging
    page_halt(pcb->parent_id);
    
    // jump to execute return
    // there is no program -> need to rerun shell
    sti();
    if (task_counter == 0) {
        printf("Restart the Base shell...\n");
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
    
    return SYSCALL_SUCCESS;
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
        if (command[i] != SPACE) {
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
    page_directory[index].pd_mb.base_addr = ((KERNEL_POSITION) + parent_id); // give the address of the process
    task_counter--; // decrement the counter

    //CP 4: remove the vid page table
    vid_page_table[0].present = 0;
    vid_page_table[0].read_write = 0;
    vid_page_table[0].user_supervisor = 0;  
    vid_page_table[0].base_addr = NULL;
    vid_page_table[0].cache_disabled = 0;
    vid_page_table[0].dirty = 0;

    // flush the TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : : : "eax", "cc"
    );
}

//Checkpoint 4 
void command_to_arg(uint8_t* arg, uint8_t* command) {
    uint32_t i;
    uint32_t j;
    memset(arg, 0, sizeof(arg));
    for (i = 0; i < 128; i++) {
        // stop at the first space
        if (command[i] == SPACE) {
            for (j = 0; j < 128; j++) {
                if (command[i + j + 1] == '\0') {
                    break;
                }
                arg[j] = command[i + j + 1];
                }
            break;
        }
    }
}

int32_t sys_getargs (uint8_t* buf, int32_t nbytes) {
    pcb_t * pcb_1;
    int32_t ret;
    int32_t i;
    int32_t j;
    int flag = 0;
    int32_t lastend;
    pcb_1 = find_pcb();

    if (pcb_1 -> fd_entry[0].flag == 0) {
        return -1;
    }
    memset(buf, NULL, nbytes);
    
    for (i = 0; i < 1024; i++) {
        // stop at the first space
        if (pcb_1->cmd[i] == SPACE || pcb_1->cmd[i] == 0) {
            for (j = 0; j < 1024; j++) {
                if (pcb_1->cmd[i + j + 1] == SPACE || pcb_1->cmd[i + j + 1] == 0) {
                    break;
                }
                buf[j] = pcb_1->cmd[i + j + 1];
                }
            break;
        }
    }

    return 0;
}



/* sys_vidmap
 * Description: maps the text-mode video memory into user space at a pre-set virtual address. 
 * address returned is always the same, it should be written into the memory location provided 
 * by the caller (which must be checked for validity). 
 * 
 * Inputs: 
 *    screen_start -- destination pointer-> the screen memory the user program specified
 * Return Value: 
 * - If the location is invalid, the call should return -1.
 * - 0, for the success address
 * Side effect: none
 */
int32_t sys_vidmap( uint8_t** screen_start){
    // check if the pointer passed in is valid
    if (screen_start == NULL || (uint32_t)screen_start < VIDMAP_LOWER_BOUND || (uint32_t)screen_start > VIDMAP_UPPER_BOUND)
        return SYSCALL_FAIL;

    uint32_t index;
    // uint32_t vm = (uint32_t)(*screen_start);

    for (index = 0; index < PAGE_ENTRY_NUMBER; index++) {
        vid_page_table[index].val = 0;
    }

    page_directory[34].pd_kb.val = ( (uint32_t)vid_page_table) | 23;

    vid_page_table[0].present = 1;
    vid_page_table[0].read_write = 1;
    vid_page_table[0].user_supervisor = 1;  
    vid_page_table[0].base_addr = (VIDEO_MEMORY >> PT_SHIFT) &(0x3ff);        // B8000 >> 12,
    vid_page_table[0].cache_disabled = 1;
    vid_page_table[0].dirty = 1;

    // SET_PT_ENTRY(vid_page_table[(vm>>12)&(0x3FF)])
    // flush the TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : : : "eax", "cc"
    );

    // memset(0x08800000, 't', 200);
    *screen_start = (uint8_t*) VIDMAP_NEW_ADDRESS;

    return 0;
}
/*Extra point, useless for now*/
int32_t sys_set_handler( int32_t signum, void* handler_address){
    return -1;
}
/*Extra point, useless for now*/
int32_t sys_sigreturn (void){
    return -1;
}

