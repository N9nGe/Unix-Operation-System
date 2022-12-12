#include "scheduling.h"
/*Global variable*/

uint32_t running_term = 0; // Term: F1, F2, F3
uint32_t schedule_counter = 1;

/* 
 *  void scheduler()
 *  DESCRIPTION: schedule between three process, set up correct program image and page usage
 *  INPUTS: none, this function is triggered by PIT, time slice is 10ms
 *  RETURN VALUE:none
 */
void scheduler(){
    /* go to the next terminal using Round Robin */
    running_term = (running_term % 3) + 1 ;
    /* start executing three shells before any other processes */
    if ( pcb_counter[0] == NULL || pcb_counter[1] == NULL || pcb_counter[2] == NULL){
        register uint32_t current_ebp asm("ebp");
        register uint32_t current_esp asm("esp");
        current_pcb_pointer->current_ebp = current_ebp;
        current_pcb_pointer->current_esp = current_esp;
        sys_execute("shell");
    } 
    /* Map the text-mode video memory to current terminal's video page in user space */

    // choose which video page should we map to (determined by the currently displaying terminal)

    /* save current ebp and esp */

    /* restore TSS of the next process */

    /* Map program image to the next process' address */


    /* flush TLB (OSdev) */

    /* switch to the next process' esp & ebp */
    
}

