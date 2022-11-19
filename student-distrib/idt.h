#ifndef _IDT_H
#define _IDT_H

#include "types.h"

#define EXCEPTION_0             0
#define EXCEPTION_1             1
#define EXCEPTION_2             2
#define EXCEPTION_3             3
#define EXCEPTION_4             4
#define EXCEPTION_5             5
#define EXCEPTION_6             6
#define EXCEPTION_7             7
#define EXCEPTION_8             8
#define EXCEPTION_9             9
#define EXCEPTION_10            10
#define EXCEPTION_11            11
#define EXCEPTION_12            12
#define EXCEPTION_13            13
#define EXCEPTION_14            14
#define EXCEPTION_15            15
#define EXCEPTION_16            16
#define EXCEPTION_17            17
#define EXCEPTION_18            18
#define EXCEPTION_19            19
// CP5
#define PIT_INTR_NUM            0x20
#define KEYBOARD_INTR_NUM       0x21
#define RTC_INTR_NUM            0x28
#define SYS_CALL_NUM            0x80

#define HANDLER_MAGIC 255

extern volatile int exception_flag;
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

// system call handler only prints "system call"
void system_call_handler (void);

// complete other modifications in idt[] when adding a new interrupt
// handler
void add_intr_handler_setup (unsigned int n);

// function that is called to initialize the IDT
void idt_init ();

#endif
