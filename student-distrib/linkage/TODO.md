# Todo list of SysCall

1.  [x] clean keyboard 

2. - [ ] 3910S> -> MentOS

3. Debug keyboard
    - Tab
    - edge case
    - will the last space make any problem?
        - print the keyboard counter at end
    - record video 
    - Design a data structure for it


4. vidmap


/*TODO: vimap
1. create a new page table pointer
2. change some value of the PDE, especially index == 33, because 10^24?
3. CR3 is the key to load into memory 
4. Line 25 of paging!
    page_directory[0].pd_kb.val = ((uint32_t) page_table) | R_W_PRESENT;

*/

5. fix the start ui


6. OS DEV: mouse choose and copy paste

Q how to 

7. gdb test the cat hello, know keyboard buf case 