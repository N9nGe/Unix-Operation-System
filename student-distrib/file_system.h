#include "x86_desc.h"
#include "lib.h"
#include "types.h"

typedef struct data_block {
    uint32_t random;
} data_block_t;

typedef struct inode {
    uint32_t random;
} inode_t;

typedef struct dentry {
    uint32_t random;
} dentry_t;

typedef struct boot_block {
    uint32_t dentry_number;
    uint32_t inode_number;
    uint32_t data_blocks;
} boot_block_t;

void file_system_init(uint32_t* fs_start);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

int32_t read_dentry_by_index(const uint8_t* fname, dentry_t* dentry);

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int file_open();

void file_read();

int file_write();

int file_close();

int dir_open();

void dir_read(int32_t fd, void* buf, int32_t nbytes);

int dir_write();

int dir_close();

