#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"
#include "file_system.h"
#include "types.h"
#include "x86_desc.h"
#include "paging.h"
#include "devices/RTC.h"
#include "devices/terminal.h"

#define USER_PROGRAM_IMAGE_START     0x08048000
#define KERNEL_BOTTOM                0x00800000
#define PROCESS_SIZE                 0X2000
#define MAX_PCB_NUM 6

// system execute
int32_t execute (const uint8_t* command);

// system halt
int32_t halt(uint8_t status);

// helper function
void parse_arg(const uint8_t* command, uint8_t* filename);
void paging_execute();

// type size?
// clearify everything in it
typedef struct pcb_t {
    uint32_t pid;         // Current Process id
    uint32_t parent_id;   // Father process
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint8_t active;
    fd_entry_t fd_entry[8];
} pcb_t;


void pcb_init (void);

#endif /* TESTS_H */

