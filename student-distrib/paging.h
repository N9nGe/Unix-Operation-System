#ifndef _PAGING_H_
#define _PAGING_H_

#include "x86_desc.h"

#define PAGE_ENTRY_NUMBER   1024
#define PAGE_SIZE           4096
#define KERNEL_POSITION     0x400000 >> 22
#define PT_SHIFT            12
#define PD_SHIFT            22
#define VIDEO_MEMORY        0xB8000
#define R_W_PRESENT         3
#define BIG_PAGE_SIZE       0x400000

#define VIDEO_PAGE_1        0xB9000
#define VIDEO_PAGE_2        0xBA000
#define VIDEO_PAGE_3        0xBB000

page_directory_t page_directory[PAGE_ENTRY_NUMBER] __attribute__((aligned(PAGE_SIZE)));
page_table_entry_t page_table[PAGE_ENTRY_NUMBER] __attribute__((aligned(PAGE_SIZE)));
page_table_entry_t vid_page_table[PAGE_ENTRY_NUMBER] __attribute__((aligned(PAGE_SIZE)));

void paging_init();

void switch_vid_page(uint8_t prev_term, uint8_t current_term);

#endif
