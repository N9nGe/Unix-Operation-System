#include "x86_desc.h"
//#include "../syscalls/ece391syscall.h"
#include "i8259.h"
#include "lib.h"
#include "idt.h"

// exceptions provided by https://wiki.osdev.org/Exceptions
// strings that will be displayed by the exception handler
char * exception_output[32] = {
    "Divide-by-zero Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved 1",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved 2",
    "Reserved 3",
    "Reserved 4",
    "Reserved 5",
    "Reserved 6",
    "Reserved 7",
    "Hypervisor Injection Exception	",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved 8"
};


void exception_handler_n (unsigned int n) {
    printf ("%s\n",exception_output[n]);
}

void divide_by_zero_exception_handler () {
    exception_handler_n (0);
}

void idt_init () {
    int i;

    // reset the entire idt
    for (i = 0; i < NUM_VEC; i++) {
        idt[i].offset_15_00 = 0;
        idt[i].seg_selector = 0;
        idt[i].reserved4 = 0;
        idt[i].offset_31_16 = 0;
    }

    SET_IDT_ENTRY(idt[0], divide_by_zero_exception_handler);
    // 32 slots for exception handler
    /*
    for (i = 0; i < 32; i++) {
        SET_IDT_ENTRY(idt[i], exception_handler(i));
    }
    */

    return;
}
