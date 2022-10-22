#include "x86_desc.h"
#include "lib.h"
#include "types.h"

#define FILENAME_LEN            32
#define DATA_BLOCK_SIZE         1023
#define DATA_BLOCK_ENTRY_SIZE   4096
#define DENTRY_RESERVED         24
#define BOOT_BLOCK_RESERVED     52
#define DENTRY_SIZE             63

typedef struct data_block {
    uint8_t entry[DATA_BLOCK_ENTRY_SIZE];  // *** changed size to 4096
} data_block_t;

typedef struct inode {
    uint32_t length;
    uint32_t data_block_num[DATA_BLOCK_SIZE];
} inode_t;

typedef struct dentry {
    uint8_t filename[FILENAME_LEN];
    uint32_t filetype;
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

void file_system_init(uint32_t* fs_start);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry);

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int file_open(uint8_t* fname);

void file_read(uint8_t* fname);

int file_write();

int file_close();

int dir_open();

void dir_read(int32_t fd, void* buf, int32_t nbytes);

int dir_write();

int dir_close();

void files_ls();

