#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "lib.h"
#include "paging.h"
#include "syscall.h"
#include "types.h"
#include "devices/keyboard.h"
#include "devices/terminal.h"

extern uint32_t running_term;

void scheduler();

#endif /* SCHEDULING_H */

