#include"pit.h"
#include"../i8259.h"

/* Programmable Interval Timer Driver
 * Author: Tony Wang -- 1 11/18
 * 
*/
#define PIT_IRQ_NUM 	0x00
#define PIT_SUCCESS 	0
#define PIT_FAIL		-1
//only need channel one, other 2 are useless
#define PIT_DATA_PORT	0x40
#define PIT_MODE_PORT	0x43
/* Mode: Channel 0, mode 3 */
#define PIT_MODE_2        0x37 	// Square wave, to seperate the mode evenly with 3 terminals      
#define PIT_FREQ_SET	0x10000 // Slowest freq, but how to judge?      
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

	outb(PIT_MODE_2,PIT_MODE_PORT);

	outb(PIT_FREQ_SET,PIT_DATA_PORT);

    sti(); // set interrupt flags
    restore_flags(flags);   // restore flags 

    // generate the interrupt by sending the signal to the PIC
    enable_irq(PIT_IRQ_NUM);
    return;
}

void pit_interrupt_handler(){
    cli();
    send_eoi(PIT_IRQ_NUM);
	// printf("hello, pit! ");
	// Used for sheduling
    sti();
}
/*
IRQ0_handler:
	push eax
	push ebx
 
	mov eax, [IRQ0_fractions]
	mov ebx, [IRQ0_ms]                    ; eax.ebx = amount of time between IRQs
	add [system_timer_fractions], eax     ; Update system timer tick fractions
	adc [system_timer_ms], ebx            ; Update system timer tick milli-seconds
 
	mov al, 0x20
	out 0x20, al                          ; Send the EOI to the PIC
 
	pop ebx
	pop eax
	iretd
*/



/*	PIT's related syscall
 *	Useless for now, but leave the IO here for further development
 *	-1 -- fail, default for unrealized functionality
 *
 * 

*/
int32_t pit_open(const uint8_t * filename){
	return -1;
}
int32_t pit_read(int32_t fd, void* buf, int32_t nbytes){
	return -1;
}
int32_t pit_write(int32_t fd, const void* buf, int32_t nbytes){
	return -1;
}
int32_t pit_close(int32_t fd){
	return -1;
}
