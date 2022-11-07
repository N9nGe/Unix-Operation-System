// /* syscall_link.h - system call linkage  
//  * Tony -- 1 PCB
//  * vim:ts=4 noexpandtab
//  */
// #ifndef _SYSCALL_LINK_H_
// #define _SYSCALL_LINK_H_

#include"../types.h"
#include"../syscall.h"

    #ifndef ASM_SL

    extern void syscall_handler();
    #endif

//#endif /* _SYSCALL_LINK_H_*/
