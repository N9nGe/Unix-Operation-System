#ifndef _PAGING_H_
#define _PAGING_H_

#include "x86_desc.h"

#define PAGE_ENTRY_NUMBER   1024
#define PAGE_SIZE           4096
#define KERNEL_POSITION     0x400000 >> 22
#define PT_SHIFT            12
#define VIDEO_MEMORY        0xB8000
#define R_W_PRESENT         3
#define PAGING_ENABLE_MASK  0x80000000
#define PAGE_4MB_ENABLE_MASK  0x00000010

page_directory_t page_directory[PAGE_ENTRY_NUMBER] __attribute__((aligned(PAGE_SIZE)));
page_table_entry_t page_table[PAGE_ENTRY_NUMBER] __attribute__((aligned(PAGE_SIZE)));


void paging_init();

#endif
