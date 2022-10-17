#include "RTC.h"

/* uint8_t rtc_init()
 * Inputs: none
 * Return Value: 0 for success
 * Function: Initialize the RTC and Turn on the IRQ8 in PIC for RTC
 */
uint8_t rtc_init() {
    // Reference link: https://wiki.osdev.org/RTC
    // Turn on the IRQ8 for RTC
    uint32_t flags;
    cli_and_save(flags);    // CLI and save EFLAGS
    outb(NMI_MASK | RTC_B_OFFSET, RTC_PORT_INDEX); // select register B, and disable NMI
    uint8_t prev = inb(RTC_PORT_CMOS);  // read the current value of register B
    outb(NMI_MASK | RTC_B_OFFSET, RTC_PORT_INDEX);  // set the index again (a read will reset the index to register B)
    outb(prev | 0x40, RTC_PORT_CMOS);  // write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti(); // set interrupt flags
    restore_flags(flags);   // restore flags 

    return 0;
}

/* uint8_t rtc_set_freq()
 * Inputs: none
 * Return Value: 0 for success
 * Function: Set the RTC frequency to 2 Hz
 */
uint8_t rtc_set_freq() {
    uint8_t rate = 0x0F;
    uint32_t flags;
    cli_and_save(flags);    // CLI and save EFLAGS
    outb(NMI_MASK | RTC_A_OFFSET, RTC_PORT_INDEX); // select register B, and disable NMI
    uint8_t prev = inb(RTC_PORT_CMOS);  // read the current value of register B
    outb(NMI_MASK | RTC_A_OFFSET, RTC_PORT_INDEX);  // set the index again (a read will reset the index to register B)
    outb((prev | 0xF0) | rate, RTC_PORT_CMOS);  // write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti();  // set interrupt flags
    restore_flags(flags);   // restore flags 
    
    return 0;
}

/* void rtc_interrupt()
 * Inputs: none
 * Return Value: none
 * Function: test interrupt and read the value from the register C
 */
void rtc_interrupt() {
    // test IRQ
    printf("%d", 1);
    // read register C to ensure interrupt happen again
    outb(NMI_MASK | RTC_C_OFFSET, RTC_PORT_INDEX); // select register C
    inb(RTC_C_OFFSET);                             // just throw away contents
    send_eoi(RTC_IRQ);

}
