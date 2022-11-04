#include "syscall.h"
#include "lib.h"

void sys_open (void) {
    printf ("sys_open called");
}

void sys_close (void) {
    printf ("sys_close called");
}

void sys_read (void) {
    printf ("sys_read called");
}

void sys_write (void) {
    printf ("sys_write called");
}
