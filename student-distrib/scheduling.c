#include "scheduling.h"

uint32_t running_term = 1; // Term: F1, F2, F3
uint32_t schedule_counter = 0;

void scheduler(){
    // TODO get pcb of the current active process (need multi-terminal) ***
    if (schedule_counter < 3) {
        schedule_counter++;
        running_term = schedule_counter;
        register uint32_t current_ebp asm("ebp");
        register uint32_t current_esp asm("esp");
        current_pcb_pointer->current_ebp = current_ebp;
        current_pcb_pointer->current_ebp = current_esp;
        sys_execute((uint8_t*)"shell");
        //terminal[running_term].running_pcb = &current_pcb_pointer;
        return;
    }
    /*if (pcb_counter[0] == 0 || pcb_counter[1] == 0 || pcb_counter[2] == 0) {
        register uint32_t current_ebp asm("ebp");
        register uint32_t current_esp asm("esp");
        current_pcb_pointer->current_ebp = current_ebp;
        current_pcb_pointer->current_ebp = current_esp;
        sys_execute((uint8_t*)"shell");
    }*/

    // go to the next terminal using Round Robin
    running_term = running_term % 3 + 1;    // from terminal 1 to 3

    pcb_t* next_pcb = terminal[running_term].running_pcb; // TODO get new pcb according to the next running terminal (need currently running proccess on every terminal)


    // /* Map video memory to the current terminal's video page */
    // if (display_term == running_term)    // TODO check variable name
    //     page_table[0].base_addr = (VIDEO_MEMORY >> PT_SHIFT);        // B8000 >> 12 
    // else    // go to the correct video page according to currently running terminal
    //     page_table[0].base_addr = ((VIDEO_MEMORY + 0x1000*running_term) >> PT_SHIFT);   // TODO check with teammate 

    // // flush TLB (OSdev)
    // asm volatile(
    //     "movl %%cr3, %%eax;" 
    //     "movl %%eax, %%cr3;"
    //     : 
    //     : 
    //     : "eax", "cc"
    // );


    /* Map the text-mode video memory to current terminal's video page */
    page_directory[VIDMAP_PAGE_INDEX].pd_kb.val = ((uint32_t)vid_page_table) | VIDMAP_MAGIC;    // VIDMAP_MAGIC == PD's present, R_W, U_S 
    vid_page_table[0].present = 1;
    vid_page_table[0].read_write = 1;
    vid_page_table[0].user_supervisor = 1;  
    // choose which video page should we map to (determined by the currently displaying terminal)
    if (display_term == running_term)    // TODO check variable name
        vid_page_table[0].base_addr = (VIDEO_MEMORY >> PT_SHIFT);        // TODO check understanding    B8000 >> 12 
    else    // go to the correct video page according to currently running terminal
        vid_page_table[0].base_addr = ((VIDEO_MEMORY + 0x1000*running_term) >> PT_SHIFT);   // TODO check with teammate     
    
    // flush TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : 
        : 
        : "eax", "cc"
    );
    


    /* Map program image to the next process' address */
    page_directory[32].pd_mb.val = 0;   // clear the entry
    page_directory[32].pd_mb.present = 1;    
    page_directory[32].pd_mb.read_write = 1;
    page_directory[32].pd_mb.page_size = 1;  // initialize 4mb page 
    page_directory[32].pd_mb.base_addr = (KERNEL_POSITION) + (next_pcb->pid + 1) * 0x400000; // provide the address of the next program

    // switch to the next process' esp & ebp
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        :
        : "r" (next_pcb->saved_esp), "r" (next_pcb->saved_ebp)
        : "esp", "ebp"
    );
    
    // restore TSS of the next process
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (next_pcb->pid - 1) - AVOID_PAGE_FAULT); // TODO CHECK

    // flush TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : 
        : 
        : "eax", "cc"
    );

}


// void 



// void shell_init(){
     
//     const uint8_t* command = "shell";
    
//     // filename buffer used by parse the command
//     uint8_t filename[FILENAME_LEN];
//     uint8_t tmp_cmd[strlen_unsigned(command)];
    
//     // initialize the filename buffer
//     memset(filename, 0, FILENAME_LEN);
//     memset(tmp_cmd, 0, strlen_unsigned(command));

//     strcpy_unsigned(tmp_cmd, command);
//     // parse the command(fill the command into the buffer)
//     parse_arg(command, filename);

//     dentry_t execute_file;
//     memset(&execute_file, 0, sizeof(execute_file));
//     // check whether file exist
//     if (read_dentry_by_name(filename, &execute_file) == SYSCALL_FAIL) {
//         return SYSCALL_FAIL;
//     }

//     // check whether the file is an EXE
//     uint8_t execute_code_buf[EXE_CODE_BUF];
//     read_data(execute_file.inode_num, 0, execute_code_buf, EXE_CODE_BUF);
//     if (execute_code_buf[0] != EXE_MAGIC_1 || execute_code_buf[1] != EXE_MAGIC_2 || 
//         execute_code_buf[2] != EXE_MAGIC_3 || execute_code_buf[3] != EXE_MAGIC_4) {
//         return SYSCALL_FAIL;
//     }
//     // check whether new pcb has the position
//     if (find_pid() == 0) {
//         printf("No position for new pcb!\n");
//         return ;
//     }

//     cli();
//     // paging the new memory
//     paging_execute(find_pid());
//     // load the file into USER_PROGRAM_IMAGE_START(virtual memory)
//     if (read_data(execute_file.inode_num, 0, (uint8_t*) USER_PROGRAM_IMAGE_START, inode_ptr[execute_file.inode_num].length) == 0) {
//         return ;
//     }
    
//     // create new pcb
//     int32_t parent_id = 0;
//     if (task_counter > 1) {
//         parent_id = current_pcb_pointer->pid;
//     }
//     parent_pcb = current_pcb_pointer;
//     pcb_t* new_pcb = pcb_initilize();
//     // itask_counter= NULL) {
//     //     printf("No position for new pcb!\n");
//     //     return SYSCALL_FAIL;
//     // }
//     current_pcb_pointer = new_pcb;
//     // update pid and parent_id
//     new_pcb->pid = find_pid();
//     new_pcb->parent_id = parent_id;
//     new_pcb->parent_pcb = parent_pcb;
//     strcpy_unsigned(new_pcb->cmd, tmp_cmd);
//     pcb_counter[new_pcb->pid - 1] = 1;
    
//     // save old ebp & esp (faom review slides)
//     register uint32_t saved_ebp asm("ebp");
//     register uint32_t saved_esp asm("esp");

//     // store ebp && esp in the pcb
//     new_pcb->saved_ebp = saved_ebp;
//     new_pcb->saved_esp = saved_esp;
    
//     // prepare for context switch(from kernel to usermode)
//     // USER_DS, ESP, EFLAG, CS, EIP
//     uint32_t user_data_segment = USER_DS;
//     // get byte 24-28 in EXE
//     uint32_t eip = *(uint32_t*)(((uint8_t*) USER_PROGRAM_IMAGE_START) + TWENTY_FOUR_OFFSET);
//     uint32_t user_code_segment = USER_CS;
//     uint32_t esp = (USER_PROGRAM_IMAGE_START - USER_PROGRAM_IMAGE_OFFSET + BIG_PAGE_SIZE - AVOID_PAGE_FAULT); // -4 because dereference is 4 byte value(avoid page fault)
    
//     // TSS
//     // no need to change ss0 because kernel using the same kernel stack(initilize at booting)
//     // get the current stack address
//     tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (new_pcb->pid - 1) - AVOID_PAGE_FAULT);
//     sti();

//     // context switch && IRET
//     asm volatile(
//         "xorl %%eax, %%eax;"
//         "movl %0, %%eax;"
//         "movw %%ax, %%ds;"
//         "pushl %0;" 
//         "pushl %1;"
//         "pushfl;"
//         "pushl %2;"
//         "pushl %3;"
//         "IRET;"
//         :
//         : "r" (user_data_segment), "r" (esp), "r" (user_code_segment), "r" (eip)
//         : "cc", "memory", "eax"
//     );

//     return ;
// }
