/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#include<stdio.h>  // TODO

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

#define MASTER_8259_DATA MASTER_8259_PORT + 1
#define SLAVE_8259_DATA  SLAVE_8259_PORT + 1
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
    unsigned long flags;  // EFLAG saving 
    master_mask = 0xff;
    slave_mask  = 0xff;
    // printf("initiliazing the PIC")
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
    

    enable_irq( ICW3_SLAVE );
    sti() // why we should call it here
    restore_flags(flags);
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
    if (irq_num < 0 || irq_num > 15){   
        printf("Failed to find corrsponding port");
        return ;
    }
    if (irq_num <8)
    {// if it is master 
        master_mask &= ~( 1 << irq_num);
        outb(master_mask,MASTER_8259_DATA);

    }else{// then it's slave
        slave_mask &= ~(1 << (irq_num-8));
        outb(slave_mask,SLAVE_8259_DATA);

    }
    
    // TODO : correct value
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
    if (irq_num < 0 || irq_num > 15){   
        printf("Failed to find corrsponding port");
        return;
    }
    if (irq_num <8)
    {// if it is master 
        master_mask |= ~(1 << irq_num);
        outb(master_mask,MASTER_8259_DATA);

    }else{// then it's slave
        slave_mask |= ~(1 << (irq_num-8));
        outb(slave_mask,SLAVE_8259_DATA);

    }
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
    if (irq_num < 0 || irq_num > 15){   
        printf("Failed to find corrsponding port");
        return ;
    }
    if (irq_num <8)
    {// if it is master 
        master_mask |= EOI;
        outb(master_mask,MASTER_8259_PORT);

    }else{// then it's slave
        slave_mask |= EOI;
        outb(slave_mask,SLAVE_8259_PORT);
        // TODO: OR WTIH 2?

    }
}
