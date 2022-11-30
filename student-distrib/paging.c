#include "paging.h"
#include "types.h"
#include "lib.h"


/* paging_init()
 * Description: initialize the paging
 * Inputs: none
 * Return Value: none
 * Function: initialize the page firectory and page table, enable the paging
 */

void paging_init() {
    uint32_t index;     // use for initialization index
    // clean all page directory and page table
    for (index = 0; index < PAGE_ENTRY_NUMBER; index++) {
        page_directory[index].pd_kb.val = 0;
    }

    for (index = 0; index < PAGE_ENTRY_NUMBER; index++) {
        page_table[index].val = 0;
    }

    // set up the read_write, present signal for the first page directory (split to 4kb)
    // and give the corresponding page table base address
    page_directory[0].pd_kb.val = ((uint32_t) page_table) | R_W_PRESENT;    // TODO

    // set up the read_write, present signal for the second page directory (kernel-4mb page)
    // and give the corresponding page table base address
    // kernel is in the second entry of the directory
    page_directory[1].pd_mb.present = 1;    
    page_directory[1].pd_mb.read_write = 1;
    page_directory[1].pd_mb.page_size = 1;  // change to 4mb page 
    page_directory[1].pd_mb.base_addr = KERNEL_POSITION; // give the address of the kernel
    // find the video memory and initialize it
    page_table[VIDEO_MEMORY >> PT_SHIFT].present = 1;
    page_table[VIDEO_MEMORY >> PT_SHIFT].read_write = 1;
    page_table[VIDEO_MEMORY >> PT_SHIFT].base_addr = (VIDEO_MEMORY >> PT_SHIFT);

    // video pages for the multiple terminal
    page_table[VIDEO_PAGE_1 >> PT_SHIFT].present = 1;
    page_table[VIDEO_PAGE_1 >> PT_SHIFT].read_write = 1;
    page_table[VIDEO_PAGE_1 >> PT_SHIFT].user_supervisor = 1;
    page_table[VIDEO_PAGE_1 >> PT_SHIFT].base_addr = (VIDEO_PAGE_1 >> PT_SHIFT);

    page_table[VIDEO_PAGE_2 >> PT_SHIFT].present = 1;
    page_table[VIDEO_PAGE_2 >> PT_SHIFT].read_write = 1;
    page_table[VIDEO_PAGE_2 >> PT_SHIFT].user_supervisor = 1;
    page_table[VIDEO_PAGE_2 >> PT_SHIFT].base_addr = (VIDEO_PAGE_2 >> PT_SHIFT);

    page_table[VIDEO_PAGE_3 >> PT_SHIFT].present = 1;
    page_table[VIDEO_PAGE_3 >> PT_SHIFT].read_write = 1;
    page_table[VIDEO_PAGE_3 >> PT_SHIFT].user_supervisor = 1;
    page_table[VIDEO_PAGE_3 >> PT_SHIFT].base_addr = (VIDEO_PAGE_3 >> PT_SHIFT);

    // enable the paging 
    asm volatile(
        /* load the page directory into the CR3 */
        "movl %0, %%eax;" 
        "movl %%eax, %%cr3;"

        /* allow the 4mb page */
        "movl %%cr4, %%eax;"
        "orl $0x00000010, %%eax;" /*PAGE_4MB_ENABLE_MASK*/ 
        "movl %%eax, %%cr4;"

        /* enable Paging */
        "movl %%cr0, %%eax;"
        "orl $0x80000000, %%eax;" /*PAGING_ENABLE_MASK*/ 
        "movl %%eax, %%cr0;"
        :                            /* output */
        :"r"(page_directory)         /* input */
        :"%eax"                      /* clobbered register */
    );

}

