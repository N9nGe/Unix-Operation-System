# syscall_link.h - Assembly linkage for device and idt
.data
    SYS_MIN  =  1
    SYS_MAX  =  10
    FAIL     =  -1
    SUCCESS  =  0
jump_table:
		.long sys_open, sys_close, sys_read, sys_write

.text

.globl syscall_handler
# return value: none
syscall_handler:                            \
    pushal                                  ;\
    pushfl                                  ;\
	              ;\
# if eax < min                  
    cmpl   $SYS_MIN, %eax
    jb    INVALID_SYS
    cmpl   $SYS_MAX, %eax
    ja    INVALID_SYS

    call *jump_table(,%ebx,4)               ;\
    popfl                                   ;\
    popal                                   ;\

    iret

    INVALID_SYS:
        movl    $FAIL, %eax
        popfl
        popal
        iret


# 0 -> dummy jump? sequence matters!
