#include "types.h"

void exception_handler_n (unsigned int n);

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

void system_call_handler (void);

void add_intr_handler_setup (unsigned int n);

void idt_init ();
