#ifndef _RTC_H_
#define _RTC_H_

#include "../student-distrib/lib.h"
#include "../student-distrib/types.h"

// The default IRQ in the PIC
#define RTC_IRQ         8

// The IO port used by the RTC
#define RTC_PORT_INDEX  0x70
#define RTC_PORT_CMOS   0x71

// The frecquency used for RTC
#define RTC_FREC        2
#define RTC_BASE_FREQ   1024

// The offset used for CMOS RAM
#define NMI_MASK        0X80
#define RTC_A_OFFSET    0x0A
#define RTC_B_OFFSET    0x0B
#define RTC_C_OFFSET    0x0C

uint8_t rtc_init();
uint8_t rtc_set_freq();
void rtc_interrupt();

#endif