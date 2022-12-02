#ifndef _RTC_H_
#define _RTC_H_

#include "../lib.h"
#include "../types.h"

// RTC return value
#define RTC_SUCCESS     0
#define RTC_FAIL        -1

// just for the test
#define RTC_TEST_COUNTER 	32

// The default IRQ in the PIC
#define RTC_IRQ         8

// The IO port used by the RTC
#define RTC_PORT_INDEX  0x70
#define RTC_PORT_CMOS   0x71

// The frecquency used for RTC
#define RTC_INIT_DEFAULT_FREQ    1024
#define RTC_OPEN_DEFAULT_FREQ    2
#define RTC_MAX_FREQ             1024
#define RTC_MIN_FREQ             2

// The offset used for CMOS RAM
#define NMI_MASK        0X80
#define RTC_A_OFFSET    0x0A
#define RTC_B_OFFSET    0x0B
#define RTC_C_OFFSET    0x0C

uint8_t rtc_init();
uint8_t rtc_set_freq();
void rtc_interrupt();

int32_t rtc_open(const uint8_t * filename);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_close(int32_t fd);

#endif
