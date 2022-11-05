# Todo list of SysCall

1. Jerry: File system's read write open close
data structure is of problem

write file & dir different?
Should be same
2. 
RTC first for testing

# syscall_link.h - Assembly linkage for device and idt
.data

.text

.globl syscall_handler
# return value: none
syscall_handler:                            \
    pushal                                  ;\
    pushfl                                  ;\
	movl	8(%esp), %ebx   #  cmd -> ebx   ;\
	cmpl	$0, %ebx                        ;\
# if ebx 0                  

    call *jump_table(,%ebx,4)               ;\
    popfl                                   ;\
    popal                                   ;\

    iret



jump_table:
		.long sys_open, sys_close, sys_read, sys_write


# 0 -> dummy jump? sequence matters!



; movl	8(%esp), %ebx   #  cmd -> ebx   ;\
	; cmpl	$0, %ebx            

3. 