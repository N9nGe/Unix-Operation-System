#define ASM_SL 1         
# syscall_link.h - Assembly linkage for device and idt
.data
    SYS_MIN  =  1
    SYS_MAX  =  6
    FAIL     =  -1
    SUCCESS  =  0
# leave other 4 in CP4
jump_table:
		.long 0x0, sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close

# .text

.globl syscall_handler
# return value: none
syscall_handler:                                                      
    /* save all registers */
    pushl %esi
    pushl %edi
    pushl %esp
    /* push parameters */
    pushl %edx
    pushl %ecx
    pushl %ebx
    /* boundary check */
    cmpl   $SYS_MIN, %eax                   
    jb    INVALID_SYS                       
    cmpl   $SYS_MAX, %eax                   
    ja    INVALID_SYS
    /* jump to corrsponding syscall */                       
    call *jump_table(,%eax,4)               
    jmp  VALID_SYS
INVALID_SYS:
    movl    $FAIL, %eax                 
VALID_SYS:
    popl %ebx
    popl %ecx
    popl %edx
    popl %esp
    popl %edi
    popl %esi
                                       
    iret                                  
