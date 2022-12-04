#include "paging.h"
#include "types.h"
#include "lib.h"

int * vid_pages[4];

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
    page_directory[0].pd_kb.val = ((uint32_t) page_table) | R_W_PRESENT;   

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

    vid_pages[0] = (int*) VIDEO_MEMORY;
    vid_pages[1] = (int*) VIDEO_PAGE_1;
    vid_pages[2] = (int*) VIDEO_PAGE_2;
    vid_pages[3] = (int*) VIDEO_PAGE_3;

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


/* void switch_vid_page(uint8_t current, uint8_t next)
 * Inputs: uint8_t current = terminal number of the current terminal (before switch)
 *         uint8_t next = terminal number of the next terminal (after switch)
 * Return Value: void
 * Function: memcopy data in displaying video page to the video page of the old terminal's
 * video page. Then memcopy data from the next terminal's video page to display video page.
 */

void switch_vid_page(uint8_t current, uint8_t next) {
    cli();
    memcpy(vid_pages[current], vid_pages[0], 4096);
    memset(vid_pages[0], NULL, 4096);
    memcpy(vid_pages[0], vid_pages[next], 4096);
    sti();
}
