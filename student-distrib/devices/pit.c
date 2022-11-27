#include"pit.h"
#include"../i8259.h"

/*
The PIT chip uses the following I/O ports:

I/O port     Usage
0x40         Channel 0 data port (read/write)
0x41         Channel 1 data port (read/write)
0x42         Channel 2 data port (read/write)
0x43         Mode/Command register (write only, a read is ignored)
*/

void pit_init(){
    // Reference link: https://wiki.osdev.org/Programmable_Interval_Timer
    // Turn on the IRQ0 for PIT
    uint32_t flags;
    cli_and_save(flags);    // CLI and save EFLAGS

	// set the squared wave mode
	outb(PIT_MODE_3, PIT_MODE_PORT);

	// set the devisor frequency(16 bit) 
	outb(PIT_OSCILLATOR_FREQ & PIT_LOW_MASK, PIT_DATA_PORT); // get the low 8 bit of the frequency
	outb(PIT_OSCILLATOR_FREQ >> 8, PIT_DATA_PORT);  // get the high 8 bit of the frequency

    sti(); // set interrupt flags
    restore_flags(flags);   // restore flags 
    // generate the interrupt by sending the signal to the PIC
    enable_irq(PIT_IRQ_NUM);
}

void pit_interrupt_handler(){
    cli();
    send_eoi(PIT_IRQ_NUM);
	//TODO: NEED TO add the multiterminal stuff to do the context switch(flag. global process...)
    sti();
}
