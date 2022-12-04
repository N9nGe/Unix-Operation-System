#include"pit.h"
#include"../i8259.h"
#include "../scheduling.h"
// static int i = 0;

/*
The PIT chip uses the following I/O ports:

I/O port     Usage
0x40         Channel 0 data port (read/write)
0x41         Channel 1 data port (read/write)
0x42         Channel 2 data port (read/write)
0x43         Mode/Command register (write only, a read is ignored)
*/

/* pit_init()
 * Description: pit initialization
 * Inputs: void
 * Return Value: void
 * Function: pit initialization when boot the system (set to 100 hz/ 10ms per interrupt)
 */
void pit_init(){
    // Reference link: https://wiki.osdev.org/Programmable_Interval_Timer
    // Turn on the IRQ0 for PIT
    uint32_t flags;
	uint32_t data;
    cli_and_save(flags);    // CLI and save EFLAGS

	// set the squared wave mode
	outb(PIT_MODE_3, PIT_MODE_PORT);

	// set the devisor frequency(16 bit)  (0.01s per interrupt.)
	data = (PIT_OSCILLATOR_FREQ & PIT_LOW_MASK);
	outb(data, PIT_DATA_PORT); // get the low 8 bit of the frequency
	data = (PIT_OSCILLATOR_FREQ >> EIGHT_BIT_SHIFT);
	outb(data, PIT_DATA_PORT);  // get the high 8 bit of the frequency

    sti(); // set interrupt flags
    restore_flags(flags);   // restore flags 
    // generate the interrupt by sending the signal to the PIC
    enable_irq(PIT_IRQ_NUM);
}

/* pit_interrupt_handler()
 * Description: pit interrupt handler
 * Inputs: void
 * Return Value: void
 * Function: support for multiprocessor.
 */
void pit_interrupt_handler(){
    cli();
    send_eoi(PIT_IRQ_NUM);
    // 0.01s per interrupt.
    // test for functinality of pit
    // if (i == 1000) {
    //     //printf("%u\n", i);
    // } 
    // i++;
	scheduler();
    sti();
}
