#ifndef _PIT_H_
#define _PIT_H_

#include"../lib.h"
#include"../types.h"

void init_PIT();

void schedule_handler();

void PIT_write();

void PIT_read();

void PIT_open();

void PIT_close();





#endif /*_PIT_H_*/
