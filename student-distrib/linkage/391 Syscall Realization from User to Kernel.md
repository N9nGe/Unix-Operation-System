# 391 Syscall: Realization from User to Kernel

## Tony 11/05/2022

[TOC]

## Ground view:

1. the realization method of MP3 system call is similar to Linux: 

- **The kernel is micro and elegant, and user program have their own program to connect to the 10 system call.**
- Everything is file, no matter it is a directory or system call function. In ***syscalls/***, here is the original code generated to the ***fsdir/*** (into our **read-only file system**)

![image-20221105170022502](C:\Users\everloom\AppData\Roaming\Typora\typora-user-images\image-20221105170022502.png)

- When choosing the specific program, here we always set a basic/**root** **SHELL** program for each terminal. And using this shell, we can excute other user level program.
- For checkpoint 2&3, there is only: **One terminal == one shell + one user program == Two PCB**
- [15个cmd命令，每个windows用户都应该知道|服务器|管理器|ipconfig_网易订阅 (163.com)](https://www.163.com/dy/article/HKMMQARI0553X5CR.html)
- 

## Usage of each program

This part is used for testing purpose, here we will follow a routine from most basic to the most complex (*more parameter for the program*)

### testprint: the basic realization of sys_execute and sys_write



### **Shell: ** always open, it is a while(1) loop, quite similar to checkpoint 2's testcase of terminal.

[ NOTE: To modify the details in shell, we may need to use ***createfs*** to regenerate the corresponding shell program ]

- [ ] [MentOS]

### **hello **:  most simple program, needs sys_read & sys_write

### **ls**: a simpler version of list function in CP2. show the files line by line

### pingpong: endless, used for testing multiple terminals

### cat: ?

### grep: ?

### sigtest: used for CP4

### fish: display a living fish on the screens



### Question: 

1. halt?

   ctrl+C to halt the program?

2. clear the screen

3. 

## What we need to do

### open: 

### close:

### read:

### write

### execute: means executing a new user program from shell. 

### Creating fake iret structure is used to return to shell's kernel space after terminating the user program. Then, we will also use iret structure created by INT to back to shell'user space.

### halt: end the function when user program send return?

To be continue..

## Edge Case:

1. shell shell shell ls
2. PCB# matters?

