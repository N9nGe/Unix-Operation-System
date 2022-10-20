#include "file_system.h"

data_block_t * data_block_ptr;

inode_t * inode_ptr;

dentry_t * dentry_ptr;

boot_block_t * boot_block_ptr;

void file_system_init(uint32_t* fs_start) {
    
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    uint32_t idx;
    for (idx = 0; idx < sizeof(dentry); idx++) {
        if (strncmp(fname, dentry[idx].filename) == 0) {
            break;
        }
    }
}

int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry){
    
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    
}

int file_open() {
    return 0;
}

void file_read() {

}

int file_write() {
    return -1;
}

int file_close() {
    return 0;
}

int dir_open() {
    return 0;
}

void dir_read(int32_t fd, void* buf, int32_t nbytes) {

}

int dir_write() {
    return -1;
}

int dir_close() {
    return 0;
}

