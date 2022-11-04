# syscall_link.h - Assembly linkage for device and idt

jump_table:
		.long sys_open, sys_close, sys_read, sys_write
