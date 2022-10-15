/* i8259.c - Functions to interact with the Keyboard interrupt device
 * 
 * Tony  1  10.14.2022
 */
#include "lib.h"
#include "keyboard.h"

/* Initialize keyboard input device */
void keyboard_init(void);
/* Enable (unmask) the specified IRQ */
void enable_key_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable__key_irq(uint32_t irq_num);