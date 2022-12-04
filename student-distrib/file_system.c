#include "file_system.h"
#include "syscall.h"

data_block_t * data_block_ptr;
inode_t * inode_ptr;
dentry_t * dentry_ptr;
boot_block_t * boot_block_ptr; 
static uint32_t temp_position;  // temporary file position 
extern pcb_t* current_pcb_pointer;

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
    data_block_ptr = (data_block_t *) (inode_ptr + DENTRY_SIZE + 1);
}

/* 
 *  read_dentry_by_name
 *  DESCRIPTION: load the information of a directory entry (filename, filetype, inode number) 
 *  inside the boot_block to the dentry pointer passed by the argument
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
        // use string compare to check if the filenames match
        if (strncmp_unsigned(fname, tmp_dentry.filename, FILENAME_LEN)== 0) {
            strcpy_unsigned(dentry -> filename, tmp_dentry.filename);
            dentry -> filetype = tmp_dentry.filetype;
            dentry -> inode_num = tmp_dentry.inode_num;
            return 0;
        }
    }
    // if the dentry is not found, return -1
    return -1;
}

/* 
 *  read_dentry_by_index
 *  DESCRIPTION: load the information of a directory entry (filename, filetype, inode number) 
 *  inside the boot_block to the dentry pointer passed by the argument
 *  INPUTS: const uint32_t index -- the index of the of the dentry that is going to be read
 *          dentry * dentry -- dentry that will be modified, where the found dentry information
 *                             is going to be stored
 *  OUTPUTS: none
 *  RETURN VALUE: -1 if failed. Otherwise, return 0
 *  SIDE EFFECTS: none
 */
int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry){
    dentry_t tmp_dentry;
    // check if the index is inside the range
    if (index < 0 || index > (DENTRY_SIZE - 1)) return -1;
    // 
    if (dentry == NULL) return -1;
    tmp_dentry = boot_block_ptr -> dir_entries[index];
    uint8_t testnull[FILENAME_LEN] = {0};
    // check if the dentry at this index is invalid
    if (strncmp_unsigned(tmp_dentry.filename, testnull, FILENAME_LEN) == 0) {
        return -1;
    }

    // fill the temp dentry 
    strcpy_unsigned(dentry -> filename, tmp_dentry.filename);
    dentry -> filetype = tmp_dentry.filetype;
    dentry -> inode_num = tmp_dentry.inode_num;
    return 0;
}

/* 
 *  read_data
 *  DESCRIPTION: 
 *  INPUTS: uint32_t inode -- inode number for the file that we are reading
            uint32_t offset -- starting position within the file
            uint8_t* buf -- the buffer that stores the data read from file
            uint32_t length -- the number of bytes that we are reading from
            the file
 *  OUTPUTS: none
 *  RETURN VALUE: number of bytes read; 0 if no byte is read
 *  SIDE EFFECTS: save the data read into the buffer passed into the function
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    // error checking
    if (inode < 0 || inode >= DENTRY_SIZE || buf == NULL || boot_block_ptr == NULL) return -1;

    uint32_t inode_startblk_idx = offset / DATA_BLOCK_ENTRY_SIZE;     // calculate which block to start with
    // uint32_t startblk_idx = offset % DATA_BLOCK_ENTRY_SIZE;           // calculate the start byte within the start block
    uint32_t inode_endblk_idx;  // the block to end with
    uint32_t endblk_idx;        // the end byte within the end block
    int32_t bytes_read = 0;    // number of bytes read

    if ((length + offset) > inode_ptr[inode].length) {  // if reading more data than the actual size of the file
        inode_endblk_idx = (inode_ptr[inode].length) / DATA_BLOCK_ENTRY_SIZE;
        endblk_idx = (inode_ptr[inode].length) % DATA_BLOCK_ENTRY_SIZE;      // end byte is the ending byte of the file
    }
    else{    // we are reading within the size of the file
        inode_endblk_idx = (length + offset) / DATA_BLOCK_ENTRY_SIZE;
        endblk_idx = (length + offset) % DATA_BLOCK_ENTRY_SIZE;
    }

    uint32_t idx, i;
    uint32_t buf_idx = 0;   // stores where we are at in the buffer
    uint32_t block_idx;     // index to the data blocks
    inode_t curr_inode = inode_ptr[inode];
    int32_t loop_start;
    int32_t loop_end;
    for (idx = inode_startblk_idx; idx <= inode_endblk_idx; idx++) {
       
        // start case
        if (idx == inode_startblk_idx && idx == inode_endblk_idx) {
            loop_start = offset % DATA_BLOCK_ENTRY_SIZE;
            loop_end = endblk_idx;
        } else if (idx == inode_startblk_idx && idx != inode_endblk_idx) {
            loop_start = offset % DATA_BLOCK_ENTRY_SIZE;
            loop_end = DATA_BLOCK_ENTRY_SIZE;
        } else if (idx != inode_startblk_idx && idx == inode_endblk_idx) {
            loop_start = 0;
            loop_end = endblk_idx;
        } else {
            loop_start = 0;
            loop_end = DATA_BLOCK_ENTRY_SIZE;
        }

        for (i = loop_start; i < loop_end; i++){
            block_idx = curr_inode.data_block_num[idx];
            // block_idx error checking
            if (block_idx < 0 || block_idx > DATA_BLOCK_SIZE)
                return -1;
            buf[buf_idx] = data_block_ptr[block_idx].entry[i];
            buf_idx++;
            bytes_read++;
        }
    }
    return (int32_t)bytes_read;
}


/* 
 *  file_open
 *  DESCRIPTION: 
 *  INPUTS: const uint8_t* filename -- open the file in the system
 *  OUTPUTS: none
 *  RETURN VALUE: -1 if failed. Otherwise, return 0
 *  SIDE EFFECTS: none
 *  NOTE: 
 *      Tony 11.8: I delete the prvious header, it dosn't fit the prototype. 
 */
int32_t file_open(const uint8_t* filename) {
    if (strlen_unsigned(filename) > FILENAME_LEN){    // return failed if file name is larger than 32B
        printf("\n!!!File open failed!\n");
        return -1;
    }
    return 0;
}

/* 
 *  file_read
 *  DESCRIPTION: 
 *  INPUTS: int32_t fd -- not used for checkpoint 3.2
 *          uint8_t* buf -- the buffer that stores the data read from file
 *          int32_t nbytes -- number of bytes to be re
 *  OUTPUTS: none
 *  RETURN VALUE: number of bytes read; 0 if no byte is read
 *  SIDE EFFECTS: save the data read into the buffer passed into the function
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
    unsigned length;
    int32_t bytes_read;
    length = inode_ptr[current_pcb_pointer->fd_entry[fd].inode_num].length;
    if (current_pcb_pointer->fd_entry[fd].file_pos >= length)  {
        //current_pcb_pointer->fd_entry[fd].file_pos = 0;
        return 0;
    }
    memset(buf, NULL, nbytes);
    // printf("fd is %d||%s",fd,(int8_t*)buf); // TEST     
    bytes_read = read_data (current_pcb_pointer->fd_entry[fd].inode_num, 
    current_pcb_pointer->fd_entry[fd].file_pos, buf, nbytes);

    // if (bytes_read > nbytes) {
    //     current_pcb_pointer->fd_entry[fd].file_pos += nbytes;
    //     return nbytes;
    // }
    current_pcb_pointer->fd_entry[fd].file_pos += bytes_read;
    return bytes_read;   // return bytes copied 
}

/* 
 *  file_write
 *  DESCRIPTION: don't do anything
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: always -1, because the filesystem is read-only
 *  SIDE EFFECTS: none
 */
int file_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/* 
 *  file_close
 *  DESCRIPTION: don't do anything yet
 *  INPUTS: int32_t fd -- file descriptor
 *  OUTPUTS: none
 *  RETURN VALUE: always 0 for now
 *  SIDE EFFECTS: none
 */
int file_close(int32_t fd) {
    return 0;
}

/* 
 *  dir_open
 *  DESCRIPTION: don't do anything yet
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: always 0 for now
 *  SIDE EFFECTS: none
 */
int dir_open(const uint8_t* fname) {
    temp_position = 0;
    // printf("dir open");
    return 0;
}

/* 
 *  dir_read
 *  DESCRIPTION: read the name of a directory one at a time
 *  INPUTS: int32_t fd -- file descriptor 
            uint8_t* buf -- buffer to store the file name into
            int32_t nbytes -- number of bytes to read
 *  OUTPUTS: none
 *  RETURN VALUE: number of bytes copied
 *  SIDE EFFECTS: read the directory entry name into buf
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes) {
    // ignoring fd, nbytes for 3.2
    int32_t bytes_copied;
    // copy filename of the current dentry to buffer
    bytes_copied = 0;
    uint32_t curr_pos = current_pcb_pointer->fd_entry[fd].file_pos;
    strncpy_unsigned((uint8_t *) buf, boot_block_ptr->dir_entries[curr_pos].filename, FILENAME_LEN);

    bytes_copied = strlen_unsigned((uint8_t *)buf);
    if (current_pcb_pointer->fd_entry[fd].file_pos < boot_block_ptr->dentry_count)
        current_pcb_pointer->fd_entry[fd].file_pos++;

    if (bytes_copied > nbytes) {
        return nbytes;
    }
    return bytes_copied;   // return bytes copied 
}

/* 
 *  dir_write
 *  DESCRIPTION: don't do anything yet
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: always -1 for now
 *  SIDE EFFECTS: none
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/* 
 *  dir_close
 *  DESCRIPTION: don't do anything yet
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: always 0 for now
 *  SIDE EFFECTS: none
 */
int32_t dir_close(int32_t fd) {
    return 0;
}

/* 
 *  files_ls
 *  DESCRIPTION: list all dentries' filename, filetype, and size
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: always 0 for now
 *  SIDE EFFECTS: none
 */
void files_ls(){ 
    uint32_t idx, i;
    uint8_t fname[FILENAME_LEN+1];
    uint32_t space_len;
    dentry_t tmp_dentry;
    uint32_t len;

    // loop through every dentry in the current directory
    for (idx = 0; idx < boot_block_ptr -> dentry_count; idx++){
        dir_read(0, fname, 0);
        read_dentry_by_name(fname, &tmp_dentry);    // find the dentry by name

        // calculate number of space to put
        if (strlen_unsigned(tmp_dentry.filename) > FILENAME_LEN)
            space_len = 0;
        else
            space_len = FILENAME_LEN - strlen_unsigned(tmp_dentry.filename);

        i = 0;
        strncpy_unsigned(fname, tmp_dentry.filename, FILENAME_LEN);

        printf("file_name:");   // print file name
        for (i = 0; i < space_len; i++) {
            printf(" ");
        }
        printf(" ");
        printf("%s, file_type: %d, file_size:",     // print file type and size
            fname, tmp_dentry.filetype); 
        printf("  ");

        len = inode_ptr[tmp_dentry.inode_num].length;   

        // calculate the number of space before the file size string
        space_len = MAX_SPACE_LEN;
        for (i = DIGIT_SPACE; space_len >= 0; i = i * DIGIT_SPACE) {
            if (len >= i) {
                space_len--;
            } else {
                break;
            }
        }

        // put space onto screen
        for (i = 0; i < space_len; i++) {
            printf(" ");
        }
        printf("%d \n", inode_ptr[tmp_dentry.inode_num].length);
    }
}


