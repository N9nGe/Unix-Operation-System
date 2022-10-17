#include "x86_desc.h"
#include "i8259.h"
#include "lib.h"
#include "idt.h"
#include "devices/RTC.h"
#include "devices/keyboard.h"
#include "interrupts/interrupt_link.h"

// exceptions provided by https://wiki.osdev.org/Exceptions
// strings that will be displayed by the exception handler
char * exception_output[20] = {
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
    "Intel Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception"
};

/* 
 * exception_handler_n ()
 * DESCRIPTION: a helper function for every exception 
 * handler because most of them have similar
 * functionality for cp1. Therefore, instead of modifying parts
 * in all exception handler, modifying exception_handler_n
 * is faster. The function prints the corresponding
 * exception output and freeze the computer using while(1)
 * 
 * INPUTS: unsigned int n -- the IDT number of exception
 *                           used to access the output string
 *                           for different exceptions
 * OUTPUTS: exception_output[n] -- string containing the exception
 *                                 message that will be printed to
 *                                 users.
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */

void exception_handler_n (unsigned int n) {
    clear();
    printf ("%s\n",exception_output[n]);
    while (1);
    
}

/*__________________________________________________________________*/

/*
 * xxx_exception ()
 * DESCRIPTION: Function definitions for a set of exception handlers called
 * xxx_exception (). They don't have other functionalities yet except calling
 * exception_handler_n (n) and passing the IDT number of the exception as argument
 * INPUT: none
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECTS: call exception_handler_n ()
 */

void divide_by_zero_exception () {
    exception_handler_n (0);
}

void debug_exception () {
    exception_handler_n (1);
}

void non_maskable_interrupt_exception () {
    exception_handler_n (2);
}

void breakpoint_exception () {
    exception_handler_n (3);
}

void overflow_exception () {
    exception_handler_n (4);
}

void bound_range_exceeded_exception () {
    exception_handler_n (5);
}

void invalid_opcode_exception () {
    exception_handler_n (6);
}

void device_not_available_exception () {
    exception_handler_n (7);
}

void double_fault_exception () {
    exception_handler_n (8);
}

void coprocessor_segment_overrun_exception () {
    exception_handler_n (9);
}

void invalid_tss_exception () {
    exception_handler_n (10);
}

void segment_not_present_exception () {
    exception_handler_n (11);
}

void stack_segment_fault_exception () {
    exception_handler_n (12);
}

void general_protection_fault_exception () {
    exception_handler_n (13);
}

void page_fault_exception () {
    exception_handler_n (14);
}

void reserved1_exception () {
    exception_handler_n (15);
}

void x87_floating_point_exception () {
    exception_handler_n (16);
}

void alignment_check_exception () {
    exception_handler_n (17);
}

void machine_check_exception () {
    exception_handler_n (18);
}

void simd_floating_point_exception () {
    exception_handler_n (19);
}

/*__________________________________________________________________*/

/*
void virtualization_exception () {
    exception_handler_n (20);
}

void control_protection_exception () {
    exception_handler_n (21);
}

void reserved2_exception () {
    exception_handler_n (22);
}

void reserved3_exception () {
    exception_handler_n (23);
}

void reserved4_exception () {
    exception_handler_n (24);
}

void reserved5_exception () {
    exception_handler_n (25);
}

void reserved6_exception () {
    exception_handler_n (26);
}

void reserved7_exception () {
    exception_handler_n (27);
}

void hypervisor_injection_exception () {
    exception_handler_n (28);
}

void vmm_communication_exception () {
    exception_handler_n (29);
}

void security_exception () {
    exception_handler_n (30);
}

void reserved8_exception () {
    exception_handler_n (31);
}
*/

void system_call_handler () {
    printf("system call");
}

void add_intr_handler_setup (unsigned int n) {
    idt[n].present = 1;
    idt[n].dpl = 0;
    idt[n].reserved3 = 0;
}

void idt_init () {
    int i;

    // reset the entire idt
    for (i = 0; i < NUM_VEC; i++) {
        idt[i].offset_15_00 = 0;
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size      = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        idt[i].present = 0;
        idt[i].offset_31_16 = 0;
    }

    // 20 slots for exception handler

    for (i = 0; i < 20; i++) {
        idt[i].dpl = 0;
        idt[i].present = 1;
    }

    SET_IDT_ENTRY(idt[0], divide_by_zero_exception);
    SET_IDT_ENTRY(idt[1], debug_exception);
    SET_IDT_ENTRY(idt[2], non_maskable_interrupt_exception);
    SET_IDT_ENTRY(idt[3], breakpoint_exception);
    SET_IDT_ENTRY(idt[4], overflow_exception);
    SET_IDT_ENTRY(idt[5], bound_range_exceeded_exception);
    SET_IDT_ENTRY(idt[6], invalid_opcode_exception);
    SET_IDT_ENTRY(idt[7], device_not_available_exception);
    SET_IDT_ENTRY(idt[8], double_fault_exception);
    SET_IDT_ENTRY(idt[9], coprocessor_segment_overrun_exception);
    SET_IDT_ENTRY(idt[10], invalid_tss_exception);
    SET_IDT_ENTRY(idt[11], segment_not_present_exception);
    SET_IDT_ENTRY(idt[12], stack_segment_fault_exception);
    SET_IDT_ENTRY(idt[13], general_protection_fault_exception);
    SET_IDT_ENTRY(idt[14], page_fault_exception);
    SET_IDT_ENTRY(idt[15], reserved1_exception);
    SET_IDT_ENTRY(idt[16], x87_floating_point_exception);
    SET_IDT_ENTRY(idt[17], alignment_check_exception);
    SET_IDT_ENTRY(idt[18], machine_check_exception);
    SET_IDT_ENTRY(idt[19], simd_floating_point_exception);
    /*
    SET_IDT_ENTRY(idt[20], virtualization_exception);
    SET_IDT_ENTRY(idt[21], control_protection_exception);
    SET_IDT_ENTRY(idt[22], reserved2_exception);
    SET_IDT_ENTRY(idt[23], reserved3_exception);
    SET_IDT_ENTRY(idt[24], reserved4_exception);
    SET_IDT_ENTRY(idt[25], reserved5_exception);
    SET_IDT_ENTRY(idt[26], reserved6_exception);
    SET_IDT_ENTRY(idt[27], reserved7_exception);
    SET_IDT_ENTRY(idt[28], hypervisor_injection_exception);
    SET_IDT_ENTRY(idt[29], vmm_communication_exception);
    SET_IDT_ENTRY(idt[30], security_exception);
    SET_IDT_ENTRY(idt[31], reserved8_exception);
    */

    SET_IDT_ENTRY(idt[0x21], keyboard_handler_linkage);
    add_intr_handler_setup(0x21);

    SET_IDT_ENTRY(idt[0x28], rtc_handler_linkage);
    add_intr_handler_setup(0x28);

    SET_IDT_ENTRY(idt[0X80], system_call_handler);
    idt[0X80].dpl = 3;
    idt[0X80].present = 1;
    
    return;
}
