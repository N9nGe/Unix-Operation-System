#ifndef SYSCALL_H
#define SYSCALL_H
#include"lib.h"
#include"file_system.h"
#include"types.h"

/*Local variable*/
#define FAIL        -1
#define SUCCESS     0

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
    uint32_t pid;         // Current Process id
    uint32_t parent_id;   // Father process
    int32_t fd;          // File descriptor, a integer index into PCB array
                     // Used for user-level programs to identify opened file
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint8_t active;
    fd_entry_t fd_entry[6];
} pcb_t;
// fd entry is in filesystem 
// int pcb_array[6] = {0,0,0,0,0,0}; // Useless for now, but may need to check status of each pcb?
// 
typedef struct file_op_t{
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
    int32_t (*read)(int32_t fd, void* buf,  int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
}file_op_t;

void pcb_init (void);



int32_t sys_open (const uint8_t* filename);

int32_t sys_close (int32_t fd);

int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);

int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes);


file_op_t set_rtc_fop();
file_op_t set_terminal_fop();
file_op_t set_file_fop();
file_op_t set_dir_fop();

#endif /* TESTS_H */
