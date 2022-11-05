#ifndef SYSCALL_H
#define SYSCALL_H
#include"lib.h"
#include"file_system.h"
#include"types.h"

//jump table file_ops struct
// typedef struct file_ops{
//   open_fn open_op;
//   close_fn close_op;
//   read_fn read_op;
//   write_fn write_op;
// }file_ops_t;

// type size?
// clearify everything in it
typedef struct pcb_t {
    int pid;         // Current Process id
    int parent_id;   // Father process
    int fd;          // File descriptor, a integer index into PCB array
                     // Used for user-level programs to identify opened file
    int saved_esp;
    int saved_ebp;
    int active;
    fd_entry_t fd_entry[6];
} pcb_t;
// fd entry is in filesystem 
// int pcb_array[6] = {0,0,0,0,0,0}; // Useless for now, but may need to check status of each pcb?
// 

void pcb_init (void);

int32_t sys_open (const uint8_t* filename);

int32_t sys_close (const uint8_t* filename);

int32_t sys_read (void);

int32_t sys_write (void);

#endif /* TESTS_H */
