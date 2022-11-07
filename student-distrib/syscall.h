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
#define MAX_PCB_NUM                  6
#define EXE_CODE_BUF                 4
#define EXE_MAGIC_1                  0x7f
#define EXE_MAGIC_2                  0x45
#define EXE_MAGIC_3                  0x4c
#define EXE_MAGIC_4                  0x46
#define TWENTY_FOUR_OFFSET           24
#define USER_PROGRAM_IMAGE_OFFSET    0x48000
#define AVOID_PAGE_FAULT             4
#define SPACE                        0x20


// system execute
int32_t sys_execute (const uint8_t* command);

// system halt
int32_t sys_halt(uint8_t status);

// helper function
void parse_arg(const uint8_t* command, uint8_t* filename);
void paging_execute();
void page_halt(uint32_t parent_id);


// Program Control Block
typedef struct pcb_t {
    uint32_t pid;          // Current Process id
    uint32_t parent_id;    // Father process
    uint32_t saved_esp;    // stack pointer save
    uint32_t saved_ebp;    // Base pointer save
    uint8_t active;        // test bit
    fd_entry_t fd_entry[8];// file descriptor entry for current pcb
} pcb_t;
/*Other CP3 structure location*/
// fd entry is in filesystem 
// fop: also in filesystem
/*Local varialbe*/
extern pcb_t* current_pcb_pointer;

pcb_t* find_pcb(void);

void pcb_init (void);

int32_t fd_entry_init(fd_entry_t* fd_entry);

int32_t find_next_fd(void);



int32_t sys_open (const uint8_t* filename);

int32_t sys_close (int32_t fd);

int32_t sys_read (int32_t fd, uint8_t* buf, int32_t nbytes);

int32_t sys_write (int32_t fd, const uint8_t* buf, int32_t nbytes);



#endif /* TESTS_H */

