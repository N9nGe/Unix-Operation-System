#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"
#include "file_system.h"
#include "x86_desc.h"
#include "paging.h"

#define USER_PROGRAM_IMAGE_START     0x08048000
// system execute
int32_t execute (const uint8_t* command);

// system halt
int32_t halt(uint8_t status);

// helper function
void parse_arg(uint8_t* command, uint8_t* filename);
void paging_execute();

#endif /* TESTS_H */
