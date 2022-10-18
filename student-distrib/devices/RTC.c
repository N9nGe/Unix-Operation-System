#include "RTC.h"
#include "../i8259.h"
#include "../lib.h"

static uint32_t log_2(uint32_t freq);
extern void test_interrupts(void);

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
    enable_irq(RTC_IRQ);
    return 0;
}

/* uint8_t rtc_set_freq()
 * Inputs: none
 * Return Value: 0 for success
 * Function: Set the RTC frequency to 2 Hz
 */
uint8_t rtc_set_freq(uint32_t freq) {
    uint32_t flags;
    uint8_t rate = 16 - log_2(freq);
    if (rate > 2 && rate < 16) {
        cli_and_save(flags);    // CLI and save EFLAGS
        outb(NMI_MASK | RTC_A_OFFSET, RTC_PORT_INDEX); // select register B, and disable NMI
        uint8_t prev = inb(RTC_PORT_CMOS);  // read the current value of register a
        outb(NMI_MASK | RTC_A_OFFSET, RTC_PORT_INDEX);  // set the index again (a read will reset the index to register B)
        outb((prev & 0xF0) | rate, RTC_PORT_CMOS);  // write the previous value ORed with 0x40. This turns on bit 6 of register B
        sti();  // set interrupt flags
        restore_flags(flags);   // restore flags 
    } 
    return 0;
}

/* void rtc_interrupt()
 * Inputs: none
 * Return Value: none
 * Function: read the value from the register C
 */
void rtc_interrupt() {
    cli();
    // test IRQ
    // test_interrupts();

    // read register C to ensure interrupt happen again
    outb(RTC_C_OFFSET, RTC_PORT_INDEX); // select register C
    inb(RTC_PORT_CMOS);                             // just throw away contents
    send_eoi(RTC_IRQ);
    sti();
}

/* uint32_t log_2(uint32_t freq)
 * Inputs: frequency
 * Return Value: the log_2 of the input freq
 * Function: convert the frequency to the log2 form
 */
uint32_t log_2(uint32_t freq) {
    uint32_t result = 0;    // result of log_2(freq)
    uint32_t current = 1; 
    while (current < freq) {
        current *= 2;
        result++;
    }
    return result;
}

