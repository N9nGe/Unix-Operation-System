#include "scheduling.h"


void scheduler(){

    // TODO get pcb of the current active process (need multi-terminal)

    int32_t esp_saved, ebp_saved;

    // save current esp and ebp
    asm volatile(
        "movl %%esp, %0"
        "movl %%ebp, %1"
        :
        : "r" (esp_saved), "r" (ebp_saved)
        :
    );

    // go to the next terminal using Round Robin
    running_term = running_term % 3 + 1;    // from terminal 1 to 3

    pcb_t* next_pcb = ; // TODO get new pcb according to the next running terminal

    /* Map to the correct video page */
    page_directory[VIDMAP_PAGE_INDEX].pd_kb.val = ( (uint32_t)vid_page_table) | VIDMAP_MAGIC;    // VIDMAP_MAGIC == PD's present, R_W, U_S 
    vid_page_table[0].present = 1;
    vid_page_table[0].read_write = 1;
    vid_page_table[0].user_supervisor = 1;  
    // choose which video page should we map to (determined by the currently displaying terminal)
    if (displaying_term == running_term)    // TODO check variable name
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
    page_directory[32].pd_mb.base_addr = KERNEL_POSITION + (next_pcb->pid + 1) * 0x400000; // provide the address of the next program

    // switch to the next process' esp & ebp
    asm volatile(
        "movl %0, %%esp"
        "movl %1, %%ebp"
        :
        : "r" (next_pcb->saved_esp), "r" (next_pcb->saved_ebp)
        :
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


