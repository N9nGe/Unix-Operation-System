#define ASM     1

.data
    PAGING_ENABLE_MASK = 0x80000000
    PAGE_4MB_ENABLE_MASK = 0x00000010

.text
.globl loadPageDirectory, enablePaging

# Load page directory by storing the page directory address 
# into CR3 register
loadPageDirectory:
    pushl %ebp
    movl %esp, %ebp

    movl 8(%esp), %eax
    movl %eax, %cr3     # storing PD address to CR3

    movl %ebp, %esp
    popl %ebp
    ret

# Enable paging by setting the 32th bit in the CR0 register;
# also enable 4 MB pages
enablePaging:
    pushl %ebp
    movl %esp, %ebp

    movl %cr0, %eax
    orl $PAGING_ENABLE_MASK, %eax   
    movl %eax, %cr0     # setting the paging bit

    movl cr4, eax      
    orl eax, PAGE_4MB_ENABLE_MASK
    movl eax, cr4        # enable 4 MB pages

    movl %ebp, %esp
    popl %ebp
    ret
