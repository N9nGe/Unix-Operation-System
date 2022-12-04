#include "syscall.h"

/*Local constant*/
#define SYSCALL_FAIL        -1
#define SYSCALL_SUCCESS     0
#define FD_MIN      2 // For read, write, open and close, stdin and stdout is check seperately
#define FD_MAX      7

pcb_t * current_pcb_pointer = (pcb_t*) (KERNEL_BOTTOM - PROCESS_SIZE);
pcb_t * parent_pcb = NULL;
//CP4: add a length-6 bitmap for pcb, ensure the tasks are fixed in 6 places
uint32_t pcb_counter[6] = {0, 0, 0, 0, 0, 0};

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
    for (i= 0; i < 2; i++) {
        fd_entry[i].file_pos = 0;
        fd_entry[i].flag = 1;
        fd_entry[i].inode_num = 0;
        fd_entry[i].filetype = 0;
    }
    fd_entry[0].fot_ptr = &terminal_stdin;
    fd_entry[1].fot_ptr = &terminal_stdout;
    /*Other fd only need to set flag*/
    for (i= 2; i < 8; i++) {
        fd_entry[i].flag = 0;
    }
    return 0;
}

/* find_pid()
 * Description: get the current pid
 * Inputs: NONE
 * Return Value: current pid
 * Function: get the current pid by the pcb counter
 */
uint32_t find_pid() {
    uint32_t pcb_bitmap = 0;
    while (pcb_counter[pcb_bitmap] != 0) {
        pcb_bitmap++;
        if (pcb_bitmap >= 6) {
            return 0;
        }
    }
    return (pcb_bitmap + 1);
}
/* find_pcb()
 * Description: get the current pcb pointer
 * Inputs: pid
 * Return Value: current pcb pointer
 * Function: get the current pcb by the pid
 */
pcb_t* find_pcb() {
    uint32_t pid = find_pid();
    if (pid == 0) {
        return NULL;
    }
    return ((pcb_t*) (KERNEL_BOTTOM - PROCESS_SIZE * (pid)));
}

/* find_next_fd()
 * Description: get the next fd
 * Inputs: none
 * Return Value: -1 for syscall_fail; i for the available fd
 * Function: get the next available fd
 */
int32_t find_next_fd() {
    pcb_t * pcb_1 = current_pcb_pointer;
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
    pcb_t * pcb_1 = current_pcb_pointer;
    // pcb_1 = find_pcb();
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
        
    dentry_t temp_dentry;
    // If failed to open the file, quit it
    if (read_dentry_by_name ((uint8_t *) filename, &temp_dentry) != 0) {
        return SYSCALL_FAIL;
    } else {
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
            int32_t ret = (*(pcb_1 -> fd_entry[fd].fot_ptr->open))(filename); 
            if (ret == -1) {
                return SYSCALL_FAIL;
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
    pcb_t * pcb_1 = current_pcb_pointer;
    // pcb_1 = find_pcb();
    // check if fd is closable
    if (fd < FD_MIN || fd > FD_MAX) {
        return SYSCALL_FAIL;
    }
    // reset the designated fd_entry to 0
    if (pcb_1 -> fd_entry[fd].flag == 0) {
        return SYSCALL_FAIL;
    }
    pcb_1->fd_entry[fd].flag = 0;
    int32_t ret = pcb_1->fd_entry[fd].fot_ptr->close(fd); 
    return ret; 

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
    pcb_t * pcb_1 = current_pcb_pointer;
    // pcb_1 = find_pcb();
    // memset(buf, 0, sizeof(buf)); // Don't remove this line, Gabriel's Grave is set here(x
    // check if fd fulfills the requirement
    // check if there's function pointer in the fd
    // check if the nbytes is larger than 0
    if (fd == 1) {
        printf("1 failed to read fd: %d\n",fd);
        return SYSCALL_FAIL;
    }
    if (fd < 0 || fd > FD_MAX || (buf == NULL || nbytes < 0)||
       (pcb_1->fd_entry[fd].flag == 0 )) {
        printf("2 failed to read fd: %d\n",fd);
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
    pcb_t * pcb_1 = current_pcb_pointer;
    // check if fd fulfills the requirement
    // check if there's function pointer in the fd
    // check if the nbytes is larger than 0
    if (fd == 0) {
        printf("failed to write fd: %d\n",fd);
        return SYSCALL_FAIL;
    }
    // sys_write is related to stdin, so we check fd == 1 case
    if (fd < 1 || fd > FD_MAX || (buf == NULL || nbytes < 0  ) ||
       (pcb_1->fd_entry[fd].flag == 0 ) ) {
        // printf("failed to write fd: %d\n",fd);
        return SYSCALL_FAIL;
    }

    /*Function code is one line the return value */
    int32_t ret = (*(pcb_1 -> fd_entry[fd].fot_ptr -> write))(fd, buf, nbytes); 
    return ret;
}

/* sys_execute (const uint8_t* command)
 * Description: system call execute
 * Inputs: const uint8_t* command
 * Return Value: 0 succeed; -1 fail
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
    // check whether new pcb has the position
    if (find_pid() == 0) {
        printf("No position for new pcb!\n");
        return 1;
    }

    cli();
    // paging the new memory
    paging_execute(find_pid());
    terminal[running_term].task_counter++;
    // load the file into USER_PROGRAM_IMAGE_START(virtual memory)
    if (read_data(execute_file.inode_num, 0, (uint8_t*) USER_PROGRAM_IMAGE_START, inode_ptr[execute_file.inode_num].length) == 0) {
        return SYSCALL_FAIL;
    }
    
    // create new pcb
    int32_t parent_id = 0;
    uint32_t new_pid = find_pid();

    // track the current terminal running process
    terminal[running_term].terminal_process_running = 1; // 1 for shell
    // more than one process in the current terminal
    if (terminal[running_term].task_counter > 1) {
        parent_id = current_pcb_pointer->pid;
        // check whether the current terminal running process is shell
        if (strncmp((int8_t*) filename, "shell", sizeof(filename)) != 0) {
            terminal[running_term].terminal_process_running = 2; // 2 for other running process
            // flag for mask type when running pingpong
            if (strncmp((int8_t*) filename, "pingpong", sizeof(filename)) == 0) {
                terminal[running_term].pingping_flag = 1;
            }
            // flag for mask type when running fish
            if (strncmp((int8_t*) filename, "fish", sizeof(filename)) == 0) {
                terminal[running_term].fish_flag = 1;
            }
            if (strncmp((int8_t*) filename, "grep", sizeof(filename)) == 0) {
                terminal[running_term].grep_flag = 1;
            }
        } else {
            terminal[running_term].terminal_process_running = 1;
            terminal[running_term].terminal_shell_counter++;
        }
    }
    parent_pcb = current_pcb_pointer;
    pcb_t* new_pcb = pcb_initilize();
    current_pcb_pointer = new_pcb;
    // CP5
    terminal[running_term].running_pcb = new_pcb;
    
    // update pid and parent_id
    new_pcb->pid = new_pid;
    new_pcb->parent_id = parent_id;
    new_pcb->parent_pcb = parent_pcb;
    strcpy_unsigned(new_pcb->cmd, tmp_cmd);
    pcb_counter[new_pcb->pid - 1] = 1;
    
    // save old(prev) ebp & esp (from review slides) in new pcb
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
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
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (new_pcb->pid - 1) - AVOID_PAGE_FAULT);
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
    pcb_t* pcb = current_pcb_pointer;
    pcb_counter[pcb->pid - 1] = 0;
    current_pcb_pointer = pcb->parent_pcb;
    // CP5
    terminal[running_term].running_pcb = pcb->parent_pcb;
    
    // point to the parent kernel stack
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (pcb->parent_id - 1) - AVOID_PAGE_FAULT); 
    
    // close the fd
    for (i = FD_MIN; i < FD_MAX+1; i++) {
        // need file close
        sys_close(i);
        pcb -> fd_entry[i].flag = 0;
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
    terminal[running_term].task_counter--;
    
    // check the parent whether is shell
    if (terminal[running_term].task_counter < 2) {
        terminal[running_term].terminal_process_running = 1;
    }
    if (terminal[running_term].terminal_shell_counter == terminal[running_term].task_counter) {
        terminal[running_term].terminal_process_running = 1;
    }

    // when fish end, reset the flag
    terminal[running_term].fish_flag = 0;
    terminal[running_term].grep_flag = 0;
    sti();
    // determine whether we exit the base shell of the current terminal
    if (terminal[running_term].task_counter == 0) {
        printf("Restart the Base shell...\n");
        terminal[running_term].terminal_process_running = 1;
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
void paging_execute(uint32_t pid) {
    // allocate the 4mb page for each process
    uint32_t index = (uint32_t) USER_PROGRAM_IMAGE_START >> PD_SHIFT;
    page_directory[index].pd_mb.present = 1;
    page_directory[index].pd_mb.read_write = 1;
    page_directory[index].pd_mb.user_supervisor = 1;
    page_directory[index].pd_mb.page_size = 1;  // change to 4mb page 
    page_directory[index].pd_mb.base_addr = ((KERNEL_POSITION) + pid); // give the address of the process
    // flush the TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : 
        : 
        : "eax", "cc"
    );
}

/* page_halt(uint32_t parent_id)
 * Description: unmap the child process, map to the parent
 * Inputs: parent_id - map the memory to the parent memory
 * Return Value: none
 * Function: map to the parent
 */
void page_halt(int32_t parent_id) {
    // allocate the 4mb page for each process
    uint32_t index = (uint32_t) USER_PROGRAM_IMAGE_START >> PD_SHIFT;
    page_directory[index].pd_mb.present = 1;
    page_directory[index].pd_mb.read_write = 1;
    page_directory[index].pd_mb.user_supervisor = 1;
    page_directory[index].pd_mb.page_size = 1;  // change to 4mb page 
    page_directory[index].pd_mb.base_addr = ((KERNEL_POSITION) + parent_id); // give the address of the process
    // task_counter--; // decrement the counter

    //CP 4: remove the vid page table
    vid_page_table[0].present = 0;
    vid_page_table[0].read_write = 0;
    vid_page_table[0].user_supervisor = 0;  
    vid_page_table[0].base_addr = NULL;

    // flush the TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : 
        : 
        : "eax", "cc"
    );
}

//Checkpoint 4 
/* command_to_arg(uint8_t* arg, uint8_t* command)
 * Description: get the argument from a command
 * Inputs: uint8_t* arg -- argument string
 *         uint8_t* command -- command string
 * Return Value: none
 */
void command_to_arg(uint8_t* arg, uint8_t* command) {
    uint32_t i;
    uint32_t j;
    memset(arg, 0, sizeof(arg));
    for (i = 0; i < CMD_LENGTH; i++) {
        // stop at the first space
        if (command[i] == SPACE) {
            for (j = 0; j < CMD_LENGTH; j++) {
                if (command[i + j + 1] == '\0') {
                    break;
                }
                arg[j] = command[i + j + 1];
                }
            break;
        }
    }
}

/* sys_getargs (uint8_t* buf, int32_t nbytes)
 * Description: get argument from pcb's command and store in buffer
 * of size nbytes
 * Inputs: uint8_t* buf -- buffer storing the argument
 *         int32_t nbytes -- size of the buffer
 * Return Value: none
 */
int32_t sys_getargs (uint8_t* buf, int32_t nbytes) {
    pcb_t * pcb_1 = current_pcb_pointer;
    int32_t i;
    int32_t j;

    if (pcb_1 -> fd_entry[0].flag == 0) {
        return SYSCALL_FAIL;
    }
    memset(buf, NULL, nbytes);
    
    for (i = 0; i < ARG_LENGTH; i++) {
        // stop at the first space
        if (pcb_1->cmd[i] == SPACE || pcb_1->cmd[i] == 0) {
            for (j = 0; j < ARG_LENGTH; j++) {
                if (pcb_1->cmd[i + j + 1] == SPACE || pcb_1->cmd[i + j + 1] == 0) {
                    break;
                }
                buf[j] = pcb_1->cmd[i + j + 1];
                }
            break;
        }
    }

    return SYSCALL_SUCCESS;
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

    for (index = 0; index < PAGE_ENTRY_NUMBER; index++) {
        vid_page_table[index].val = 0;
    }
    // VIDMAP_MAGIC == PD's present, R_W, U_S 
    page_directory[VIDMAP_PAGE_INDEX].pd_kb.val = ( (uint32_t)vid_page_table) | VIDMAP_MAGIC;

    vid_page_table[VIDPAGE_IDX].present = 1;
    vid_page_table[VIDPAGE_IDX].read_write = 1;
    vid_page_table[VIDPAGE_IDX].user_supervisor = 1;  

    // choose which video page should we map to (determined by the currently displaying terminal)
    if (display_term == running_term)
        vid_page_table[VIDPAGE_IDX].base_addr = (VIDEO_MEMORY >> PT_SHIFT);        // B8000 >> 12 
    else    // map to the correct video page according to currently running terminal
        vid_page_table[VIDPAGE_IDX].base_addr = ((VIDEO_MEMORY + VIDPAGE_SIZE*running_term) >> PT_SHIFT);


    // flush the TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : 
        : 
        : "eax", "cc"
    );

    // Set a new page location for user program to display video
    *screen_start = (uint8_t*) VIDMAP_NEW_ADDRESS;

    return SYSCALL_SUCCESS;
}
/*Extra point, useless for now*/
int32_t sys_set_handler( int32_t signum, void* handler_address){
    return SYSCALL_FAIL;
}
/*Extra point, useless for now*/
int32_t sys_sigreturn (void){
    return SYSCALL_FAIL;
}

