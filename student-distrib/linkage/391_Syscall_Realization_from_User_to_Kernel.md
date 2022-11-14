# 391 Syscall: Realization from User to Kernel

## Tony 11/05/2022

[TOC]

## Ground view:

1. the realization method of MP3 system call is similar to Linux: 

- **The kernel is micro and elegant, and user program have their own program to connect to the 10 system call.**
- Everything is file, no matter it is a directory or system call function. In ***syscalls/***, here is the original code generated to the ***fsdir/*** (into our **read-only file system**)

![image-20221105170022502](C:\Users\everloom\AppData\Roaming\Typora\typora-user-images\image-20221105170022502.png)

- When choosing the specific program, here we always set a basic/**root** **SHELL** program for each terminal. And using this shell, we can execute other user level program.
- For checkpoint 2&3, there is only: **One terminal == one shell + one user program == Two PCB**
- [15个cmd命令，每个windows用户都应该知道|服务器|管理器|ipconfig_网易订阅 (163.com)](https://www.163.com/dy/article/HKMMQARI0553X5CR.html)
- 

## Usage of each program

This part is used for testing purpose, here we will follow a routine from most basic to the most complex (*more parameter for the program*)

### testprint: the basic realization of sys_execute and sys_write



### **Shell: ** always open, it is a while(1) loop, quite similar to checkpoint 2's testcase of terminal.

[ NOTE: To modify the details in shell, we may need to use ***createfs*** to regenerate the corresponding shell program ]

- [ ] [MentOS]

### hello :  most simple program, needs sys_read & sys_write

### ls: a simpler version of list function in CP2. show the files line by line

### pingpong: endless, used for testing multiple terminals

### cat: open file failed this time 

### grep: 

### sigtest: used for CP4

### fish: display a living fish on the screens



### Question: 

1. halt?

   ctrl+C to halt the program?

2. clear the screen
-  ctrl + l is okey, but maybe more intellgent

3. 

## What we need to do

### open: 

### close:

### read:

### write: 

### execute: means executing a new user program from shell. 

### Creating fake iret structure is used to return to shell's kernel space after terminating the user program. Then, we will also use iret structure created by INT to back to shell'user space.

### halt: end the function when user program send return?

To be continue..

## Edge Case:

1. shell shell shell ls
2. PCB# matters?

Helpful information from former students:
================================

createfs
    This program takes a flat source directory (i.e. no subdirectories
    in the source directory) and creates a filesystem image in the
    format specified for this MP.  Run it with no parameters to see
    usage.

elfconvert
    This program takes a 32-bit ELF (Executable and Linking Format) file
    - the standard executable type on Linux - and converts it to the
    executable format specified for this MP.  The output filename is
    <exename>.converted.

fish/
	This directory contains the source for the fish animation program.
	It can be compiled two ways - one for your operating system, and one
	for Linux using an emulation layer.  The Makefile is currently set
	up to build "fish" for your operating system using the elfconvert
	utility described above.  If you want to build a Linux version, do
	"make fish_emulated".  You can then run fish_emulated as superuser
	at a standard Linux console, and you should see the fish animation.

fsdir/
	This is the directory from which your filesystem image was created.
	It contains versions of cat, fish, grep, hello, ls, and shell, as
	well as the frame0.txt and frame1.txt files that fish needs to run.
	If you want to change files in your OS's filesystem, modify this
	directory and then run the "createfs" utility on it to create a new
	filesystem image.


student-distrib/
    This is the directory that contains the source code for your
    operating system.  Currently, a skeleton is provided that will build
    and boot you into protected mode, printing out various boot
    parameters.  Read the INSTALL file in that directory for
    instructions on how to set up the bootloader to boot this OS.

syscalls/
    This directory contains a basic system call library that is used by
    the utility programs such as cat, grep, ls, etc.  The library
    provides a C interface to the system calls, much like the C library
    (libc) provides on a real Linux/Unix system.  A few support
    functions have also been written (things like strlen, strcpy, etc.)
    that are used by the utility programs.  The Makefile is set up to
	build these programs for your OS.