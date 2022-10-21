#include "file_system.h"

data_block_t * data_block_ptr;  // TODO: need to set

inode_t * inode_ptr;    // TODO: need to set

dentry_t * dentry_ptr;  // not needed?

boot_block_t * boot_block_ptr;  // TODO: need to set

void file_system_init(uint32_t* fs_start) {
    // point the boot_block_ptr to the head of system_start
    boot_block_ptr = (boot_block_t *) fs_start;
    // point the dentry_ptr to the head of directory entries array in bootblock
    dentry_ptr = (dentry_t *) boot_block_ptr -> dir_entries;
    // point the inode_ptr to the head of inode array
    // inode and boot_block have same size, therefore add the size of boot_block_t
    // to the pointer will automatically skip the first boot_block
    inode_ptr = (inode_t *) (fs_start + sizeof(boot_block_t));
    // point the data block to the head of data block array using the similar
    // method to inode_ptr
    data_block_ptr = (data_block_t *) (fs_start + sizeof(boot_block_ptr) +  (boot_block_ptr -> inode_count) * sizeof(inode_t));
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    uint32_t i;
    dentry_t tmp_dentry;
    if (fname == NULL || dentry == NULL) {
        return -1;
    }
    for (i = 0; i < sizeof(boot_block_ptr -> dir_entries); i++) {
        tmp_dentry = boot_block_ptr -> dir_entries[i];
        if (strncmp(fname, tmp_dentry.filename, FILENAME_LEN)== 0) {
            dentry -> filename = tmp_dentry.filename;
            dentry -> filetype = tmp_dentry.filetype;
            dentry -> inode_num = tmp_dentry.inode_num;
            break;
        }
    }
    return 0;
}

int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry){
    dentry_t tmp_dentry;
    if (index < 0 || index > 62) return -1;
    if (dentry == NULL) return -1;
    tmp_dentry = boot_block_ptr -> dir_entries[index];
    dentry->filename = tmp_dentry.filename;
    dentry->filetype = tmp_dentry.filetype;
    dentry->inode_num = tmp_dentry.inode_num;
    return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    // uint32_t node_len = inode_ptr[inode].length;
    uint32_t inode_startblk_idx = offset / 4096;     // calculate which block to start with
    uint32_t startblk_idx = offset % 4096;               // calculate the start index within the start block
    uint32_t inode_endblk_idx;
    uint32_t endblk_idx;
    // const uint32_t buffer_size = length / 4096 + 1;
    data_block_t temp_buffer[4095];

    // calculate which block to end with 
    if (length > inode_ptr[inode].length)
        inode_endblk_idx = inode_ptr[inode].length / 4096;
    else
        inode_endblk_idx = (length + offset) / 4096;

    endblk_idx = (length + offset) % 4096;      // calculate the end index within the end block
    
    uint32_t idx, i;
    for (idx = inode_startblk_idx; idx < inode_endblk_idx; ++idx){
        if (idx == inode_startblk_idx){
            
        }
        else if (idx == inode_endblk_idx){
            
        }
        else {
            for (i = 0; i < 1023; ++i){
                temp_buffer[idx-inode_startblk_idx].entry[i] = data_block_ptr[idx];
            }
            
        }
    }
    
}

int file_open(uint8_t* fname) {
    dentry_t tmp_dentry;
    inode_t tmp_inode;
    read_dentry_by_name (fname, &tmp_dentry);
    //tmp_inode = tmp_dentry.
    return 0;
}

void file_read() {

}

int file_write() {
    return -1;
}

int file_close() {
    inode_ptr = (inode_t *) (fs_start + sizeof(boot_block_t));
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

