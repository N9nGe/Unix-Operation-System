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
#define MAX_PCB_NUM     6

//jump table file_ops struct
// typedef struct file_ops{
//   open_fn open_op;
//   close_fn close_op;
//   read_fn read_op;
//   write_fn write_op;
// }file_ops_t;

// type size?
// clearify everything in it
/*typedef struct pcb_t {
    uint32_t pid;         // Current Process id
    uint32_t parent_id;   // Father process
    int32_t fd;          // File descriptor, a integer index into PCB array
                     // Used for user-level programs to identify opened file
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint8_t active;
    fd_entry_t fd_entry[6];
} pcb_t;*/

// system execute
int32_t sys_execute (const uint8_t* command);

// system halt
int32_t sys_halt(uint8_t status);

// helper function
void parse_arg(const uint8_t* command, uint8_t* filename);
void paging_execute();
void page_halt(uint32_t parent_id);


// clearify everything in it
typedef struct pcb_t {
    uint32_t pid;         // Current Process id
    uint32_t parent_id;   // Father process
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint8_t active;
    fd_entry_t fd_entry[8];
} pcb_t;

// fd entry is in filesystem 
// fop: also in filesystem

extern int test_fd;

pcb_t* find_pcb(void);

void pcb_init (void);

int32_t fd_entry_init(fd_entry_t* fd_entry);

int32_t find_next_fd(void);



int32_t sys_open (const uint8_t* filename);

int32_t sys_close (int32_t fd);

int32_t sys_read (int32_t fd, uint8_t* buf, int32_t nbytes);

int32_t sys_write (int32_t fd, const uint8_t* buf, int32_t nbytes);



#endif /* TESTS_H */

