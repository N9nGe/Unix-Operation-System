#include "syscall.h"
#include "file_system.h"

// process counter
static int process_counter = 0;

// system execute
int32_t execute (const uint8_t* command){
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
    
    // check whether the file is valid(???)

    // paging the new memory
    paging_execute();
    // load the file into USER_PROGRAM_IMAGE_START(virtual memory)
    if (read_data(execute_file.inode_num, 0, (uint8_t*) USER_PROGRAM_IMAGE_START, inode_ptr[execute_file.inode_num].length) == 0) {
        return -1;
    }
    
    // create new pcb!!!

    // save old ebp & esp (from review slides)
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    
    // prepare for context switch!!!

    // IRET

    return 0;
}

// system halt
int32_t halt(uint8_t status){

    return 0;
}

/* parse_arg(uint8_t* command, uint8_t* filename)
 * Description: parse the argument from the commond by the first space
 * Inputs: uint8_t* command: 
 *         uint8_t* filename: 
 * Return Value: none
 * Function: copy the argument into the filename buffer to use later
 */
void parse_arg(uint8_t* command, uint8_t* filename){
    uint8_t i;
    for (i = 0; i < FILENAME_LEN; i++) {
        // stop at the first space
        if (command[i] != " ") {
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
    page_directory[index].pd_mb.page_size = 1;  // change to 4mb page 
    page_directory[index].pd_mb.base_addr = KERNEL_POSITION + 1 + process_counter; // give the address of the process
    process_counter++; // increment the counter
    // flush the TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : : : "eax", "cc"
    );
}

