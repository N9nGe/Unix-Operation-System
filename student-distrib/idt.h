#ifndef _IDT_H
#define _IDT_H

#include "types.h"

// helper function for exception handler
void exception_handler_n (unsigned int n);


/*__________________________________________________________________*/

// define a set of exception handler. The functionality of each exception
// handler is written in the name of the function

void divide_by_zero_exception (void);

void debug_exception (void);

void non_maskable_interrupt_exception (void);

void breakpoint_exception (void);

void overflow_exception (void);

void bound_range_exceeded_exception (void);

void invalid_opcode_exception (void);

void device_not_available_exception (void);

void double_fault_exception (void);

void coprocessor_segment_overrun_exception (void);

void invalid_tss_exception (void);

void segment_not_present_exception (void);

void stack_segment_fault_exception (void);

void general_protection_fault_exception (void);

void page_fault_exception (void);

void reserved1_exception (void);

void x87_floating_point_exception (void);

void alignment_check_exception (void);

void machine_check_exception (void);

void simd_floating_point_exception (void);

/*__________________________________________________________________*/

/*
void virtualization_exception ();

void control_protection_exception ();

void reserved2_exception ();

void reserved3_exception ();

void reserved4_exception ();

void reserved5_exception ();

void reserved6_exception ();

void reserved7_exception ();

void hypervisor_injection_exception ();

void vmm_communication_exception ();

void security_exception ();

void reserved8_exception ();
*/

// system call handler only prints "system call"
void system_call_handler (void);

// complete other modifications in idt[] when adding a new interrupt
// handler
void add_intr_handler_setup (unsigned int n);

// function that is called to initialize the IDT
void idt_init ();

#endif
