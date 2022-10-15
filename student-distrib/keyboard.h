/* keyboard.h - Defines used in interactions with the Keyboard interrupt
 * 
 * Tony  1  10.14.2022
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H
//todo
#include "types.h"

/* Ports that each PIC sits on */


/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI                 0x60

/* Externally-visible functions */

/* Initialize keyboard input device */
void keyboard_init(void);
/* Enable (unmask) the specified IRQ */
void enable_key_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable__key_irq(uint32_t irq_num);



#endif /* _KEYBOARD_H */
