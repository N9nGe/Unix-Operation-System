#include "file_system.h"

static data_block_t * data_block_ptr;
static inode_t * inode_ptr;
static dentry_t * dentry_ptr;
static boot_block_t * boot_block_ptr; 
static pcb_t temp_pcb;  // create a temporary pcb for 3.2
static uint32_t temp_position;  // temporary file position 

/* 
 *  file_system_init
 *  DESCRIPTION: Initialize the file system, find the corresponding pointer positions
 *  of boot_block, dentry_ptr, inode_ptr, and data_block_ptr from fs_start
 *  INPUTS: uint32_t* fs_start -- fs_start is a pointer that contains the array of 
 *                                boot_block, inodes, and data_blocks
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none
 */
void file_system_init(uint32_t* fs_start) {
    // point the boot_block_ptr to the head of system_start
    boot_block_ptr = (boot_block_t *) fs_start;

    // point the dentry_ptr to the head of directory entries array in bootblock
    dentry_ptr = (dentry_t *) boot_block_ptr -> dir_entries;

    // point the inode_ptr to the head of inode array
    // inode and boot_block have same size, therefore add the size of boot_block_t
    // to the pointer will automatically skip the first boot_block
    inode_ptr = (inode_t *) (boot_block_ptr + 1);

    // point the data block to the head of data block array using the similar
    // method to inode_ptr
    data_block_ptr = (data_block_t *) (inode_ptr + 64);
}

/* 
 *  read_dentry_by_name
 *  DESCRIPTION: load the information of a directory entry (filename, filetype, inode number) 
 *  inside theeeeee boot_block to the dentry pointer passed by the argument
 *  INPUTS: const uint8_t* fname -- a string (char array) containing the name of the file that
 *                                  is going to be read
 *          dentry * dentry -- dentry that will be modified, where the found dentry information
 *                             is going to be stored
 *  OUTPUTS: none
 *  RETURN VALUE: -1 if failed. Otherwise, return 0
 *  SIDE EFFECTS: none
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    uint32_t i;
    dentry_t tmp_dentry;
    // null check if either of arguments are null
    if (fname == NULL || dentry == NULL) {
        // return -1 if arguments are invalid
        return -1;
    }
    // go through every dentries in the boot_block's 
    for (i = 0; i < sizeof(boot_block_ptr -> dir_entries); i++) {
        tmp_dentry = boot_block_ptr -> dir_entries[i];
        if (strncmp(fname, tmp_dentry.filename, FILENAME_LEN)== 0) {
            strcpy(dentry -> filename, tmp_dentry.filename);
            dentry -> filetype = tmp_dentry.filetype;
            dentry -> inode_num = tmp_dentry.inode_num;
            return 0;
        }
    }
    return -1;
}

int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry){
    dentry_t tmp_dentry;
    if (index < 0 || index > 62) return -1;
    if (dentry == NULL) return -1;
    tmp_dentry = boot_block_ptr -> dir_entries[index];
    strcpy(dentry -> filename, tmp_dentry.filename);
    dentry -> filetype = tmp_dentry.filetype;
    dentry -> inode_num = tmp_dentry.inode_num;
    return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    // error checking
    if (inode < 0 || inode > 62 || buf == NULL || boot_block_ptr == NULL) return -1;

    uint32_t inode_startblk_idx = offset / 4096;     // calculate which block to start with
    uint32_t startblk_idx = offset % 4096;           // calculate the start byte within the start block
    uint32_t inode_endblk_idx;
    uint32_t endblk_idx;

    // calculate which block to end with 
    if (length > inode_ptr[inode].length)
        inode_endblk_idx = inode_ptr[inode].length / 4096;
    else
        inode_endblk_idx = (length + offset) / 4096;

    endblk_idx = (inode_ptr[inode].length + offset) % 4096;      // calculate the end byte within the end block
    
    uint32_t idx, i;
    uint32_t buf_idx = 0;   // stores where we are at in the buffer
    uint32_t block_idx;     // index to the data blocks
    uint32_t bytes_read = 0;    // number of bytes read
    inode_t curr_inode = inode_ptr[inode];
    
    // loop through each data_block_num entry in the current inode
    for (idx = inode_startblk_idx; idx <= inode_endblk_idx; ++idx){

        // if it's the first block
        if (idx == inode_startblk_idx){ 
            for (i = startblk_idx; i < 4096; ++i){
                block_idx = curr_inode.data_block_num[idx];
                // block_idx error checking
                if (block_idx < 0 || block_idx > 1023)
                    return -1;
                // copy data of the first block (from startblk_idx to the end of this block) to buffer 
                buf[buf_idx] = data_block_ptr[block_idx].entry[i];  // SOMETHING WRONG HERE
                buf_idx++;
                bytes_read++;
            }
        }

        // if it's the last block
        else if (idx == inode_endblk_idx){  
            // copy the data of the last block (from 0 to endblk_idx) to buffer
            for (i = 0; i < endblk_idx; ++i){
                block_idx = curr_inode.data_block_num[idx];
                // block_idx error checking
                if (block_idx < 0 || block_idx > 1023)
                    return -1;
                buf[buf_idx] = data_block_ptr[block_idx].entry[i];
                buf_idx++;
                bytes_read++;
            }
        }

        // if it's not the first or last block, copy the entire block
        else {  
            for (i = 0; i < 4096; ++i){
                block_idx = curr_inode.data_block_num[idx];
                // block_idx error checking
                if (block_idx < 0 || block_idx > 1023)
                    return -1;
                buf[buf_idx] = data_block_ptr[block_idx].entry[i];
                buf_idx++;
                bytes_read++;
            }
        }
    }
    return bytes_read;
}

int file_open(const uint8_t* fname) { 
    if (strlen(fname) > 32){    // return failed if file name is larger than 32B
        printf("File open failed!");
        return -1;
    }
    dentry_t tmp_dentry;
    read_dentry_by_name (fname, &tmp_dentry); 
    temp_pcb.inode_num = tmp_dentry.inode_num; 
    temp_pcb.flag = 1;     // set to in-use

    return 0;
}

uint32_t file_read(int32_t fd, uint8_t* buf, int32_t nbytes) {
    unsigned i;
    uint32_t bytes_read;
    if (fd < 2 || fd > 7) return -1;
    bytes_read = read_data (temp_pcb.inode_num, 0, buf, inode_ptr[temp_pcb.inode_num].length);

    for (i = 0; i < inode_ptr[temp_pcb.inode_num].length; i++) {
        printf("%c", buf[i]);
    }
    
    return bytes_read;
}

int file_write() {
    return -1;
}

int file_close(int32_t fd) {
    temp_pcb.flag = 0;     // set to unused
    return 0;
}

int dir_open() {
    // temporary for 3.2
    temp_position = 0;
    return 0;
}

uint32_t dir_read(int32_t fd, uint8_t* buf, int32_t nbytes) {
    // ignoring fd, nbytes for 3.2
    uint32_t bytes_copied;
    if (fd < 2 || fd > 7) return -1;
    strncpy(buf, boot_block_ptr->dir_entries[temp_position].filename, 32);
    bytes_copied = strlen(buf);

    if (temp_position < boot_block_ptr->dentry_count)
        temp_position++;

    return bytes_copied;   // return bytes copied 
}


int dir_write() {
    return -1;
}

int dir_close() {
    return 0;
}

void files_ls(){ 
    uint32_t idx, ch, i;
    uint8_t fname[33];
    uint8_t* space_len;
    uint8_t ftype, inode_num;
    dentry_t tmp_dentry;
    uint32_t len;

    for (idx = 0; idx < boot_block_ptr -> dentry_count; idx++){
        dir_read(0, fname, 0);
        read_dentry_by_name(fname, &tmp_dentry);

        if (strlen(tmp_dentry.filename) > 32)
            space_len = 0;
        else
            space_len = 32 - strlen(tmp_dentry.filename);

        i = 0;
        strncpy(fname, tmp_dentry.filename, 32);

        printf("file_name:");
        for (i = 0; i < space_len; i++) {
            printf(" ");
        }
        printf(" ");
        printf("%s, file_type: %d, file_size:", 
            fname, tmp_dentry.filetype); 
        printf("  ");

        len = inode_ptr[tmp_dentry.inode_num].length;
        if (len < 10000 && len >= 1000) {
            space_len = 1;
        } else if (len < 1000 && len >= 100) {
            space_len = 2;
        } else if (len < 100 && len >= 10) {
            space_len = 3;
        } else if (len < 10 && len >= 0) {
            space_len = 4;
        } else {
            space_len = 0;
        }
        for (i = 0; i < space_len; i++) {
            printf(" ");
        }
        printf("%d \n", inode_ptr[tmp_dentry.inode_num].length);
    }
}


