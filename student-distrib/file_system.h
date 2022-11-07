#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "x86_desc.h"
#include "lib.h"
#include "types.h"


#define FILENAME_LEN            32
#define DATA_BLOCK_SIZE         1023
#define DATA_BLOCK_ENTRY_SIZE   4096
#define DENTRY_RESERVED         24
#define BOOT_BLOCK_RESERVED     52
#define DENTRY_SIZE             63
#define DIGIT_SPACE             10
#define MAX_SPACE_LEN           4
#define LARGE_BUF_SIZE          9999
#define LONG_FILE_SIZE          5277
#define HALF_FRAME0             93
#define FILENAME_LEN            32
#define FILE_COUNT              17

typedef struct data_block {
    uint8_t entry[DATA_BLOCK_ENTRY_SIZE];  // *** changed size to 4096
} data_block_t;

typedef struct inode {
    uint32_t length;
    uint32_t data_block_num[DATA_BLOCK_SIZE];
} inode_t;

typedef struct dentry {
    uint8_t filename[FILENAME_LEN];
    uint32_t filetype;      // 0 for 
    uint32_t inode_num;     // inode number
    uint8_t reserved[DENTRY_RESERVED];
} dentry_t;

typedef struct boot_block {
    uint32_t dentry_count;          // number of direct entries
    uint32_t inode_count;           // number of inodes
    uint32_t data_blocks_count;     // number of data blocks
    uint8_t reserved[BOOT_BLOCK_RESERVED];
    dentry_t dir_entries[DENTRY_SIZE];
} boot_block_t;
// Checkpoint 3
/*File Operation Table*/
typedef struct file_op_t{
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
    int32_t (*read)(int32_t fd, void* buf,  int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
}file_op_t;
/*file desctiptor entry*/
typedef struct fd_entry_t {
    file_op_t* fot_ptr;         // file operations jump table pointer
    uint32_t inode_num;         // inode number for this file
    uint32_t file_pos;          // position within the file
    uint32_t flag;              // 1 indicates in-use; 0 indicates  unused
    int  filetype;              // filetype, used for selecting specific type
} fd_entry_t;
/*An old structure used for debug*/
// typedef struct tmp_pcb {
//     uint32_t fot_ptr;       // file operations jump table pointer
//     uint32_t inode_num;     // inode number for this file
//     uint32_t file_pos;      // position within the file
//     uint32_t flag;         // 1 indicates in-use; 0 indicates unused
// } tmp_pcb_t;


extern data_block_t * data_block_ptr;
extern inode_t * inode_ptr;
extern dentry_t * dentry_ptr;
extern boot_block_t * boot_block_ptr; 

void file_system_init(uint32_t* fs_start);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry);

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t file_open(const uint8_t* fname);

int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t file_close(int32_t fd);

int32_t dir_open(const uint8_t* fname);

int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);

int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t dir_close(int32_t fd);

void files_ls();

#endif /* FILE_SYSTEM_H */
