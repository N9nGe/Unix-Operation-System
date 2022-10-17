/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#include "types.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */


/* 
 * i8259_init
 *  DESCRIPTION: Initialize the 8259 PIC
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none
 */
// about 14 col
void i8259_init(void) {
    // printf("Initializing the PIC...\n");
    unsigned long flags;  // EFLAG saving 
    // First mask all irq port to disabled
    master_mask = 0xff;   
    slave_mask  = 0xff;
    cli_and_save(flags);
    outb(master_mask,MASTER_8259_DATA);
    outb(slave_mask,SLAVE_8259_DATA);
    // First initilize MASTER pic
    outb(ICW1,MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW3_MASTER,MASTER_8259_DATA);
    outb(ICW4,MASTER_8259_DATA);
    // Second comes to SLAVE pic
    outb(ICW1,SLAVE_8259_PORT);
    outb(ICW2_SLAVE,SLAVE_8259_DATA);
    outb(ICW3_SLAVE,SLAVE_8259_DATA);
    outb(ICW4,SLAVE_8259_DATA);

    
    outb(master_mask, MASTER_8259_DATA);
    outb(slave_mask, SLAVE_8259_DATA);
    
    
    enable_irq( SLAVE_IRQ ); // Cascade to slave
    sti(); // why we should call it here
    restore_flags(flags);
    // printf("Finished initializing the PIC\n");
}


/* 
 * enable_irq
 *  DESCRIPTION: enable the specific port on i8259,
 *       accept the interrup specified by irq_num
 *  INPUTS: irq_num -- interrupt request id, 0 - 15
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: Enable (unmask) the specified IRQ
 */
// about 6
void enable_irq(uint32_t irq_num) {
    // printf("Enabling the irq #%d\n",irq_num);
    if (irq_num < IRQ_NUM_MIN || irq_num > IRQ_NUM_MAX){    
        printf("Failed to find corrsponding port\n");
        return ;
    }
    if (irq_num <PORTS_LIMIT)
    {// if it is master 
        master_mask &= ~( 1 << irq_num);
        outb(master_mask, MASTER_8259_DATA);

    }else{// then it's slave
        slave_mask &= ~(1 << (irq_num - PORTS_LIMIT));
        outb(slave_mask, SLAVE_8259_DATA);

    }
    return;

}
/* 
 * disable_irq
 *  DESCRIPTION: disable the specific port on i8259,
 *  to stop accepting the interrup specified by irq_num
 *  INPUTS: irq_num -- interrupt request id, 0 - 15
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: Disable (mask) the specified IRQ
 */

// about 6 
void disable_irq(uint32_t irq_num) {
    // printf("Disabling the irq #%d\n",irq_num);
    if (irq_num < IRQ_NUM_MIN || irq_num > IRQ_NUM_MAX){   
        printf("Failed to find corrsponding port\n");
        return;
    }
    if (irq_num < PORTS_LIMIT)
    {// if it is master 
        master_mask |= 1 << irq_num;
        outb(master_mask,MASTER_8259_DATA);

    }else{// then it's slave
        slave_mask |= 1 << (irq_num- PORTS_LIMIT);
        outb(slave_mask,SLAVE_8259_DATA);
    }
    return;
}
/* 
 * send_eoi
 *  DESCRIPTION: OR EOI signal with the port on i8259,
 *  specified by irq_num
 *  INPUTS: irq_num -- interrupt request id, 0 - 15
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: Send end-of-interrupt signal for the specified IRQ
 */
// about 5
void send_eoi(uint32_t irq_num) {
    if (irq_num < IRQ_NUM_MIN || irq_num > IRQ_NUM_MAX){   
        printf("\nFailed to find corrsponding port\n");
        return ;
    }
    if (irq_num < PORTS_LIMIT)
    {// if it is master 
        irq_num |= EOI;
        outb(irq_num,MASTER_8259_PORT);

    }else{// then it's slave
        irq_num -= PORTS_LIMIT; // Here we must minus the irqnum with 8
        irq_num |= EOI;         // So slave can find right ports
        outb(irq_num,SLAVE_8259_PORT);
        outb(EOI | SLAVE_IRQ,MASTER_8259_PORT);
        // Here we need to acknowledge the master pic 

    }
}
