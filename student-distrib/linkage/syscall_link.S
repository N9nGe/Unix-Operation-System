# syscall_link.h - Assembly linkage for device and idt
.data
    SYS_MIN  =  1
    SYS_MAX  =  6
    FAIL     =  -1
    SUCCESS  =  0
# leave other 4 in CP4
jump_table:
		.long 0x0 sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close

.text

.globl syscall_handler
# return value: none
syscall_handler:                            \
    pushal                                  ;\
    pushfl                                  ;\ 
# Is it necessary to push flag?	              
# if eax < min                  
    cmpl   $SYS_MIN, %eax                   ;\
    jb    INVALID_SYS                       ;\
    cmpl   $SYS_MAX, %eax                   ;\
    ja    INVALID_SYS                       ;\

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
