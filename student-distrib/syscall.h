#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"
#include "file_system.h"
#include "types.h"
#include "x86_desc.h"
#include "paging.h"
#include "devices/RTC.h"
#include "devices/terminal.h"
#include "idt.h"
#include "scheduling.h"

/*CP3*/
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

/*CP4*/
#define ARG_LENGTH                  1024
#define CMD_LENGTH                  128
#define VIDMAP_UPPER_BOUND          0x08400000
#define VIDMAP_LOWER_BOUND          0x08000000
#define VIDMAP_NEW_ADDRESS          0x08400000

#define VIDMAP_PAGE_INDEX           33
#define VIDMAP_MAGIC                7
#define VIDPAGE_IDX                 0

/*MentOS System Calls*/
// Return value:
//     -  Success -- 0
//     -  Fail    -- -1
//Checkpoint 3
int32_t sys_halt(uint8_t status);

int32_t sys_execute (const uint8_t* command);

int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);

int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes);

int32_t sys_open (const uint8_t* filename);

int32_t sys_close (int32_t fd);
//Checkpoint 4
void command_to_arg(uint8_t* arg, uint8_t* command);

int32_t sys_getargs(uint8_t* buf, int32_t nbytes);

int32_t sys_vidmap( uint8_t** screen_start);
/*Extra point, useless for now*/
int32_t sys_set_handler( int32_t signum, void* handler_address);
/*Extra point, useless for now*/
int32_t sys_sigreturn (void);


// helper function
void parse_arg(const uint8_t* command, uint8_t* filename);
void paging_execute(uint32_t pid);
void page_halt(int32_t parent_id);

// Program Control Block
typedef struct pcb_t {
    uint32_t pid;          // Current Process id
    int32_t parent_id;     // parent process
    uint32_t saved_esp;    // stack pointer save
    uint32_t saved_ebp;    // Base pointer save
    uint8_t active;        // test bit
    uint8_t cmd[128];
    fd_entry_t fd_entry[8];// file descriptor entry for current pcb
    struct pcb_t * parent_pcb;
    uint32_t current_esp;    // stack pointer save
    uint32_t current_ebp;    // Base pointer save
} pcb_t;
/*Other CP3 structure location*/
// fd entry is in filesystem 
// fop: also in filesystem
/*Local varialbe*/
extern pcb_t* current_pcb_pointer;

extern uint32_t pcb_counter[6];

uint32_t find_pid();

pcb_t* find_pcb(void);

void pcb_init (void);

int32_t fd_entry_init(fd_entry_t* fd_entry);

int32_t find_next_fd(void);

#endif /* SYSCALL_H */

