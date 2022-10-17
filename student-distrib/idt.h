#include "types.h"

void exception_handler_n (unsigned int n);

void divide_by_zero_exception ();

void debug_exception ();

void non_maskable_interrupt_exception ();

void breakpoint_exception ();

void overflow_exception ();

void bound_range_exceeded_exception ();

void invalid_opcode_exception ();

void device_not_available_exception ();

void double_fault_exception ();

void coprocessor_segment_overrun_exception ();

void invalid_tss_exception ();

void segment_not_present_exception ();

void stack_segment_fault_exception ();

void general_protection_fault_exception ();

void page_fault_exception ();

void reserved1_exception ();

void x87_floating_point_exception ();

void alignment_check_exception ();

void machine_check_exception ();

void simd_floating_point_exception ();

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

void add_int_handler_setup (unsigned int n);

void idt_init ();
