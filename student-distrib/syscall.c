#include "syscall.h"

// no need linkage include
// jump tables for open, close, read, write

// process counter
static uint32_t task_counter = 0;
// static uint32_t parent_id = -1;

// // inilize all pcb(total 6 pcb)
// void pcb_init () {
//     uint8_t* pcb_top = (uint8_t*) KERNEL_BOTTOM - PROCESS_SIZE * MAX_PCB_NUM;
//     memset(pcb_top, 0, PROCESS_SIZE * MAX_PCB_NUM);
// }

// pcb_t* find_pcb(uint32_t pid) {
//     pcb_t* current_pcb = (pcb_t*) KERNEL_BOTTOM - PROCESS_SIZE;
//     if (current_pcb->active == 0) {
//         current_pcb->pid = pid;

//         return current_pcb;
//     } else {
//         current_pcb -= PROCESS_SIZE;
//     }
//     return NULL;
// }

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
        fd_entry->file_pos = 0;
        fd_entry->flag = 0;
        fd_entry->fot_ptr = NULL;
        fd_entry->inode_num = 0;
    }
    fd_entry[0].flag = 1;
    fd_entry[0].fot_ptr = &terminal_stdin;
    fd_entry[1].flag = 1;
    fd_entry[1].fot_ptr = &terminal_stdout;

    return 0;
}

/* find_pcb()
 * Description: get the current pcb
 * Inputs: none
 * Return Value: current pcb pointer
 * Function: get the current pcb by the task counter
 */
pcb_t* find_pcb() {
    return ((pcb_t*) (KERNEL_BOTTOM - PROCESS_SIZE * (task_counter)));
}

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

/* execute (const uint8_t* command)
 * Description: system call execute
 * Inputs: const uint8_t* command
 * Return Value: 0 succeff; -1 fail
 * Function: do the execute based on the command
 */
int32_t execute (const uint8_t* command){
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
        "orl  $0x200, %%eflags;"
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
int32_t halt(uint8_t status){
    // get current pcb
    int i;
    cli();
    pcb_t* pcb = find_pcb();
    pcb->active = 0;
    // point to the parent kernel stack
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (pcb->parent_id - 1) - 4); 
    // restore parent paging
    page_halt(pcb->parent_id);
    // TODO: using system call close (close the fd)
    for (i = 0; i < 8; i++) {
        // need file close
        pcb->fd_entry[i].flag = 0;
    }

    // TODO: check halt reason 
    if (status == 0) {
        status = 256;
    }  

    // jump to execute return
    // there is no program -> need to rerun shell
    if (task_counter == 0) {
        execute((uint8_t*)"shell");
    } else {
        asm volatile (
            "movl %0, %%eax;"
            "movl %1, %%ebp;" 
            "movl %2, %%esp;"
            "leave;"
            "ret;"
            :
            : "r" (status), "r" (pcb->saved_ebp), "r" (pcb->saved_esp)
            : "esp", "ebp", "eax"
        );
    } 
    sti();
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
    page_directory[index].pd_mb.base_addr = ((KERNEL_POSITION) + parent_id + 1); // give the address of the process
    task_counter--; // decrement the counter
    // flush the TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : : : "eax", "cc"
    );
}

