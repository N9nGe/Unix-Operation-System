#include "x86_desc.h"
#include "i8259.h"
#include "lib.h"
#include "idt.h"
#include "devices/RTC.h"
#include "devices/keyboard.h"
#include "devices/pit.h"
#include "linkage/interrupt_link.h"
#include "linkage/syscall_link.h"
#include "syscall.h"
volatile int exception_flag = 0;

// exceptions provided by https://wiki.osdev.org/Exceptions
// strings that will be displayed by the exception handler
char * exception_output[EXCEPTION_19 + 1] = {
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
    printf ("===============   %s    ===============\n  ",exception_output[n]);
    exception_flag = 1;
    sys_halt((uint8_t)HANDLER_MAGIC); // Use syshalt to support exception now
    // while(1); // Previous ending
    
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
    exception_handler_n (EXCEPTION_0);
}

void debug_exception () {
    exception_handler_n (EXCEPTION_1);
}

void non_maskable_interrupt_exception () {
    exception_handler_n (EXCEPTION_2);
}

void breakpoint_exception () {
    exception_handler_n (EXCEPTION_3);
}

void overflow_exception () {
    exception_handler_n (EXCEPTION_4);
}

void bound_range_exceeded_exception () {
    exception_handler_n (EXCEPTION_5);
}

void invalid_opcode_exception () {
    exception_handler_n (EXCEPTION_6);
}

void device_not_available_exception () {
    exception_handler_n (EXCEPTION_7);
}

void double_fault_exception () {
    exception_handler_n (EXCEPTION_8);
}

void coprocessor_segment_overrun_exception () {
    exception_handler_n (EXCEPTION_9);
}

void invalid_tss_exception () {
    exception_handler_n (EXCEPTION_10);
}

void segment_not_present_exception () {
    exception_handler_n (EXCEPTION_11);
}

void stack_segment_fault_exception () {
    exception_handler_n (EXCEPTION_12);
}

void general_protection_fault_exception () {
    exception_handler_n (EXCEPTION_13);
}

void page_fault_exception () {
    exception_handler_n (EXCEPTION_14);
}

void reserved1_exception () {
    exception_handler_n (EXCEPTION_15);
}

void x87_floating_point_exception () {
    exception_handler_n (EXCEPTION_16);
}

void alignment_check_exception () {
    exception_handler_n (EXCEPTION_17);
}

void machine_check_exception () {
    exception_handler_n (EXCEPTION_18);
}

void simd_floating_point_exception () {
    exception_handler_n (EXCEPTION_19);
}

/*__________________________________________________________________*/

void system_call_handler () {
    printf("system call");

}

/*
 * add_intr_handler_setup (unsigned int n)
 * DESCRIPTION: Change the struct of the corresponding
 * idt descriptor entry [n] when a new interrupt handler
 * is added in
 * INPUT: unsigned int n -- the IDT number where the new
 *                          interrupt handler is added.
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECTS: changed reserved3 to 0 in idt[] in x86_desc.h
 */

void add_intr_handler_setup (unsigned int n) {
    // dpl 0 means kernel level, present means there's an handler
    // at this IDT position
    idt[n].present = 1;
    idt[n].dpl = 0;
    idt[n].reserved3 = 0;
    // change reserved3 to 0 to prevent general protection error
}

/*
 * idt_init ()
 * DESCRIPTION: initialize IDT when the system is booted
 * by adding all exception and interrupt handlers to IDT
 * using (SET_IDT_ENTRY)
 * INPUT: none
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECTS: SET_IDT_ENTRY() from x86_desc.h
 */

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

    // 20 slots in IDT for exception handler

    for (i = 0; i < 20; i++) {
        // dpl 0 means kernel level, present means there's an handler
        // at this IDT position
        idt[i].dpl = 0;
        idt[i].present = 1;
        idt[i].reserved3 = 1;
        // change reserved3 to 0 to prevent general protection error
    }

    // use SET_IDT_ENTRY function provided by x86_desc.h
    // to add all exception defined on the top with corresponding
    // IDT number
    SET_IDT_ENTRY(idt[EXCEPTION_0], divide_by_zero_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_1], debug_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_2], non_maskable_interrupt_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_3], breakpoint_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_4], overflow_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_5], bound_range_exceeded_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_6], invalid_opcode_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_7], device_not_available_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_8], double_fault_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_9], coprocessor_segment_overrun_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_10], invalid_tss_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_11], segment_not_present_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_12], stack_segment_fault_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_13], general_protection_fault_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_14], page_fault_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_15], reserved1_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_16], x87_floating_point_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_17], alignment_check_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_18], machine_check_exception);
    SET_IDT_ENTRY(idt[EXCEPTION_19], simd_floating_point_exception);

    // add interrupt handlers for keyboard and rtc to the IDT
    // and complete corresponding setup in idt array
    SET_IDT_ENTRY(idt[PIT_INTR_NUM], pit_handler_linkage);
    add_intr_handler_setup(PIT_INTR_NUM);

    SET_IDT_ENTRY(idt[KEYBOARD_INTR_NUM], keyboard_handler_linkage);
    add_intr_handler_setup(KEYBOARD_INTR_NUM);

    SET_IDT_ENTRY(idt[RTC_INTR_NUM], rtc_handler_linkage);
    add_intr_handler_setup(RTC_INTR_NUM);

    // add system call to the IDT at 0x80
    SET_IDT_ENTRY(idt[SYS_CALL_NUM], syscall_handler);
    idt[SYS_CALL_NUM].dpl = 3;
    idt[SYS_CALL_NUM].present = 1;
    idt[SYS_CALL_NUM].reserved3 = 1;
    
    return;
}
