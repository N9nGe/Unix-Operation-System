/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#include<stdio.h>

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */
// Can we use it directly?
spin_lock_t i8259_lock;

/* Initialize the 8259 PIC */
// about 14 col
void i8259_init(void) {
    unsigned long flags;
    int eoi; // ??
    // spin_lock_irqsave(&i8259_lock,flags);
    outb(0xff,0x21);
    outb(0xff,0xA1);
    /*outb_p - this should work on a wide range of PC device */
    outb_p(ICW1,MASTER_8259_PORT);
    outb_p(ICW2_MASTER + 0, 0x21);
    outb_p(ICW3_MASTER,0x21);

    outb_p(ICW1,SLAVE_8259_PORT);
    outb_p(MASTER_8259_PORT + 8,SLAVE_8259_PORT +1);
    outb_p(ICW3_SLAVE,0xA1);
    outb_p(ICW4,0xA1);
    if (eoi){
        disable_irq(   );
    }else{
        // mask and ack PIC
    }
    outb(cached 21, 0x21);
    outb(cached A1, 0xA1);
    
    // spin_unlock_irqstore(&i8259_lock,flags);
}

/* Enable (unmask) the specified IRQ */
// about 6
void enable_irq(uint32_t irq_num) {
}

/* Disable (mask) the specified IRQ */
// about 6 
void disable_irq(uint32_t irq_num) {
}

/* Send end-of-interrupt signal for the specified IRQ */
// about 5
void send_eoi(uint32_t irq_num) {
}
