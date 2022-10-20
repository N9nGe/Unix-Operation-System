#include "RTC.h"
#include "../i8259.h"
#include "../lib.h"

static uint32_t log_2(uint32_t freq);
extern void test_interrupts(void);
static uint32_t current_rate = 2;

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
 * Function: Set the RTC frequency to (power of 2) Hz
 *           frequency = 2^15(32768) Hz -> rate = 1      by function: freq = 2^16 / 2^(rate - 1)
 *                       2^14(16384) Hz -> rate = 2
 *                       2^13(8192)  Hz -> rate = 3
 *                       2^12(4096)  Hz -> rate = 4
 *                       2^11(2048)  Hz -> rate = 5
 *                       2^10(1024)  Hz -> rate = 6
 *                       2^9(512)    Hz -> rate = 7
 *                       2^8(256)    Hz -> rate = 8
 *                       2^7(128)    Hz -> rate = 9
 *                       2^6(42)     Hz -> rate = 10
 *                       2^5(32)     Hz -> rate = 11
 *                       2^4(16)     Hz -> rate = 12
 *                       2^3(8)      Hz -> rate = 13
 *                       2^2(4)      Hz -> rate = 14
 *                       2^1(2)      Hz -> rate = 15
 */
uint8_t rtc_set_freq(uint32_t freq) {
    uint32_t flags;
    uint8_t rate = 16 - log_2(freq);
    // the upper bound of the frequnce is 2^13(8192) Hz -> upper bound rate is 3(Appendix B, doc pg. 19, last paragraph)
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
    inb(RTC_PORT_CMOS);                 // just throw away contents
    send_eoi(RTC_IRQ);
    sti();
}

/* uint32_t log_2(uint32_t freq)
 * Inputs: frequency
 * Return Value: the log_2 of the input freq
 * Function: convert the frequency to the log2 form
 */
uint32_t log_2(uint32_t freq) {
    // freq should be power of two
    // invalid input freq, no change for the rtc
    if (freq & (freq - 1) != 0) {
        return current_rate;
    }

    uint32_t result = 0;    // result of log_2(freq)
    uint32_t current = 1; 
    while (current < freq) {
        current *= 2;
        result++;
    }
    current_rate = result;
    return result;
}

/* int32_t rtc_open(const uint8_t * filename)
 * Inputs: filename  (//TODO: no need currently, not sure for later checkpoint)
 * Return Value: 0 for success, -1 for the named file does not exist or no descriptors are free
 * Function: convert the frequency to the log2 form
 */
int32_t rtc_open(const uint8_t * filename) {
    // pass the default freq = 2Hz into the rtc
    rtc_set_freq(2);
    return 0;
}

int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {

    return 0;
}

int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {

}

int32_t rtc_close(int32_t fd) {

}