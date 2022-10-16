#include "paging.h"
#include "types.h"
#include "paging.S"

// define the page directory and page table
page_directory_t page_directory[PAGE_ENTRY_NUMBER] __attribute__((aligned(PAGE_SIZE)));
page_table_entry_t page_table[PAGE_ENTRY_NUMBER] __attribute__((aligned(PAGE_SIZE)));
extern void loadPageDirectory(uint32_t addr);
extern void enablePaging();

/* void paging_init();
 * Description: initialize the paging
 * Inputs: none
 * Return Value: none
 * Function: initialize the page firectory and page table, enable the paging
 */

void paging_init(){
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
    page_directory[1].pd_mb.present = 1;    
    page_directory[1].pd_mb.read_write = 1;
    page_directory[1].pd_mb.page_size = 1;  // change to 4mb page 
    page_directory[1].pd_mb.base_addr = KERNEL_POSITION; // give the address of the kernel
    
    // initialize the page table
    for (index = 0; index < PAGE_ENTRY_NUMBER; index++) {
        // find the video memory
        if (index == VIDEO_MEMORY >> PT_SHIFT) {
            page_table[index].present = 1;
            page_table[index].read_write = 1;
            page_table[index].base_addr = VIDEO_MEMORY >> PT_SHIFT;
        } 
    }

    // enable the paging
    loadPageDirectory((uint32_t) page_directory);
    enablePaging();
}

