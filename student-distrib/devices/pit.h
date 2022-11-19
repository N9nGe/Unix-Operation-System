#ifndef _PIT_H_
#define _PIT_H_

#include"../lib.h"
#include"../types.h"



void pit_init();

void pit_interrupt_handler();





int32_t pit_open(const uint8_t * filename);
int32_t pit_read(int32_t fd, void* buf, int32_t nbytes);
int32_t pit_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t pit_close(int32_t fd);

#endif /*_PIT_H_*/
