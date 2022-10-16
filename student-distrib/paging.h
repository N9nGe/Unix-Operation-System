#ifndef _PAGING_H_
#define _PAGING_H_

#include "lib.h" 
#include "types.h"
#include "x86_desc.h"

#define PAGE_ENTRY_NUMBER   1024
#define PAGE_SIZE           4096
#define KERNEL_POSITION     0x400000 >> 22
#define PT_SHIFT            12
#define VIDEO_MEMORY        0xB8000
#define R_W_PRESENT         3

void paging_init();

#endif
