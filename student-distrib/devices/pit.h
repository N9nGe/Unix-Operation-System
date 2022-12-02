#ifndef _PIT_H_
#define _PIT_H_

#include"../lib.h"
#include"../types.h"

#define PIT_IRQ_NUM 		  0x00
#define PIT_SUCCESS 		  0
#define PIT_FAIL			  -1
//only need channel one, other 2 are useless
#define PIT_DATA_PORT		  0x40
#define PIT_MODE_PORT		  0x43
/* Mode: Channel 0, mode 3 */
#define PIT_MODE_3      	  0x36 	// Square wave, to seperate the mode evenly with 3 terminals      
#define PIT_FREQ			  100 	// required freq is 100Hz 
#define PIT_OSCILLATOR_FREQ   ((1193180 / PIT_FREQ) + 1)  
#define PIT_LOW_MASK          0xFF  
#define EIGHT_BIT_SHIFT       8

void pit_init();

void pit_interrupt_handler();

#endif /*_PIT_H_*/
