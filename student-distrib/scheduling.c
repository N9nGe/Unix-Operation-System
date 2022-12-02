#include "scheduling.h"

uint32_t running_term = 0; // Term: F1, F2, F3
uint32_t schedule_counter = 1;

void scheduler(){
    running_term = running_term % 3 + 1;    // from terminal 1 to 3
    
    if (pcb_counter[0] == 0 || pcb_counter[1] == 0 || pcb_counter[2] == 0) {
        register uint32_t current_ebp_tmp asm("ebp");
        register uint32_t current_esp_tmp asm("esp");
        current_pcb_pointer->current_ebp = current_ebp_tmp;
        current_pcb_pointer->current_esp = current_esp_tmp;
        sys_execute((uint8_t*)"shell");
        return;
    }
    
    // go to the next terminal using Round Robin
    register uint32_t current_ebp_tmp asm("ebp");
    register uint32_t current_esp_tmp asm("esp");
    current_pcb_pointer->current_ebp = current_ebp_tmp;
    current_pcb_pointer->current_esp = current_esp_tmp;
    pcb_t* next_pcb = terminal[running_term].running_pcb; // TODO get new pcb according to the next running terminal (need currently running proccess on every terminal)
    current_pcb_pointer = next_pcb;

    // restore TSS of the next process
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (current_pcb_pointer->pid - 1) - AVOID_PAGE_FAULT);
    tss.ss0 = KERNEL_DS;

    /* Map program image to the next process' address */
    page_directory[32].pd_mb.val = 0;   // clear the entry
    page_directory[32].pd_mb.user_supervisor = 1;
    page_directory[32].pd_mb.present = 1;    
    page_directory[32].pd_mb.read_write = 1;
    page_directory[32].pd_mb.page_size = 1;  // initialize 4mb page 
    page_directory[32].pd_mb.base_addr = (KERNEL_POSITION) + next_pcb->pid; // provide the address of the next program
    
    // flush TLB (OSdev)
    asm volatile(
        "movl %%cr3, %%eax;" 
        "movl %%eax, %%cr3;"
        : 
        : 
        : "eax", "cc"
    );

    // switch to the next process' esp & ebp
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        :
        : "r" (current_pcb_pointer->current_esp), "r" (current_pcb_pointer->current_ebp)
        : "esp", "ebp"
    );
    
}

