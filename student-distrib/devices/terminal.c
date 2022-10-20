#include"../lib.h"
#include"../types.h"
#include"keyboard.h"
#include"terminal.h"


int32_t terminal_read(int32_t fd, void* buf, uint32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);