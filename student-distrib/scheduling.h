#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "lib.h"
#include "paging.h"
#include "syscall.h"
#include "types.h"
#include "devices/keyboard.h"
#include "devices/terminal.h"

#define VIDPAGE_SIZE    0x1000
#define TERM_NUM        3
#define PROG_IMG        32

extern uint32_t running_term;

void scheduler();
void shell_init();
void vidmap_schedule();
void prog_img_schedule();

#endif /* SCHEDULING_H */

