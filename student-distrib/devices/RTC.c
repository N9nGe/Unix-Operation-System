#include "RTC.h"
#include "../i8259.h"
#include "../lib.h"

static uint32_t log_2(uint32_t freq);
extern void test_interrupts(void);
// the current frequency of the rtc (if the input freq is invalid, use the current freq)
static uint32_t current_freq;
// the flag to represent the rtc flag (Appendix B: last paragraph Use simple volatile flag variables to do this synchronization)
static volatile int rtc_interrupt_occurred; 

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

    // by the osdev, the initial rtc frequency is 1024Hz
    current_freq = RTC_INIT_DEFAULT_FREQ;

    sti(); // set interrupt flags
    restore_flags(flags);   // restore flags 

    // generate the interrupt by sending the signal to the PIC
    enable_irq(RTC_IRQ);
    return RTC_SUCCESS;
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
    // frequency =  32768 >> (rate-1) 
    uint8_t rate = 16 - log_2(freq);
    // the upper bound of the frequnce is 2^13(8192) Hz -> upper bound rate is 3 (Appendix B, doc pg. 19, last paragraph)
    //TODO: rate &= 0x0F;			// rate must be above 2 and not over 15 (from osdev)
    if (rate > 2 && rate < 16) { // rate must be above 2 and not over 15 (from osdev)
        cli_and_save(flags);    // CLI and save EFLAGS

        outb(NMI_MASK | RTC_A_OFFSET, RTC_PORT_INDEX); // set index to register A, disable NMI
        uint8_t prev = inb(RTC_PORT_CMOS);  // get initial value of register A
        outb(NMI_MASK | RTC_A_OFFSET, RTC_PORT_INDEX);  // get initial value of register A
        outb((prev & 0xF0) | rate, RTC_PORT_CMOS);  //write only our rate to A. Note, rate is the bottom 4 bits.

        // when the new input freq is in the range of rtc reasonable frequency(2 ~ 8192), change the frequency
        if (current_freq != freq) {
            current_freq = freq;
        }

        sti();  // set interrupt flags
        restore_flags(flags);   // restore flags 
    } 

    return RTC_SUCCESS;
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
    
    // rtc interrupt occurs -> set the flag to 1
    rtc_interrupt_occurred = 1;
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
    // invalid input freq, no change for the rtc frequency
    if ((freq & (freq - 1)) != 0) {
        freq = current_freq;
    }

    uint32_t result = 0;    // result of log_2(freq)
    uint32_t current = 1; 
    while (current < freq) {
        current *= 2;
        result++;
    }
    return result;
}

/* int32_t rtc_open(const uint8_t * filename)
 * Inputs: filename  (//TODO: no need currently, not sure for later checkpoint)
 * Return Value: 0 for success, -1 for the named file does not exist or no descriptors are free
 * Function: use for open system call to rtc
 */
int32_t rtc_open(const uint8_t * filename) {
    // check the whether the input is valid
    if (filename == NULL) {
        return RTC_FAIL;
    }
    // pass the default freq = 2Hz into the rtc
    rtc_set_freq(RTC_OPEN_DEFAULT_FREQ);
    return RTC_SUCCESS;
}

/* int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: int32_t fd -- file descriptor
 *         void* buf -- teh buffer contains the input frequency
 *         int32_t nbytes -- the number of byte for buffer
 * 
 * Return Value: 0 for success(always)
 * Function: For the real-time clock (RTC), this system call should always return 0, but only after an interrupt has
 *           occurred (set a flag and wait until the interrupt handler clears it, then return 0). (Appendix B: read system call)
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    //TODO: in the document it says always return success(what should be return if buf is a null)
    if (buf == NULL) {  
        return 0;
    }

    // set the rtc flag become 0(rtc interrupt not occur -> do not return)
    rtc_interrupt_occurred = 0;
    while(rtc_interrupt_occurred == 0); // if rtc interrupt occur, the flag will be set to one and then break the while loop. 
    return RTC_SUCCESS;
}

/* int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
 * Inputs: int32_t fd -- file descriptor
 *         void* buf -- teh buffer contains the input frequency
 *         int32_t nbytes -- the number of byte for buffer
 * Return Value: 0 for success, -1 for the invaild length of input frequency or the invalid pointer of buffer
 * Function: 
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    // NULL check and the rtc can only recieve the 4 byte integer specifying the interrupt rate in Hz
    if (buf == NULL || nbytes != 4 ) { 
        return RTC_FAIL;
    }
    
    // write the frequency into the rtc(Appendix B: should block interrupts to interact with the device, last paragraph)
    uint32_t flags;
    cli_and_save(flags);
    // change to the uint32 pointer first and then get the value
    rtc_set_freq(*((uint32_t*) buf));
    sti();
    restore_flags(flags);
    return RTC_SUCCESS;
}

/* int32_t rtc_close(int32_t fd)
 * Inputs: fd  
 * Return Value: 0 for success, -1 for trying to close an invalid descriptor
 * Function: for the system call to rtc to close the specified file descriptor and makes fd available
 */
int32_t rtc_close(int32_t fd) {
    // do nothing (//TODO: what the meaning of the RTC virtualization?)
    return RTC_SUCCESS;
}

