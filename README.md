# ECE391: MentOS

--- 

By Team09
- Gabriel Gao
- Tim Zhao
- Tony Wang
- Jerry Wang

1. Extra Credit ! very useful
- 10% of MP3, can cover other parts( if MP3 == 100% ) 
	- At most 5 points in total mark
	- Curve and competition makes it hard 
- finish MP3 first
- very difficult, for A+ students
	- Possible:
		-  
	- not okey:
		- a start up screen
		- fancy BSOD( Blue screen of death) 
		-   	
2. 3.2 Overview
- Keyboard driver ~= ECE385's keyboard
	- 1 people
	- all alphnumeric keys and symbols
	- exclude number pad, home/end
	- shift, ctrl, alt(checkpoint 5), capslock( locked! one time), backspace ....
		Q: how about delete? 
	- User name ? [391OS]> 
		- realized already in syscalls/ece391shell.c , it just need to be called when driver is ready
		- stored in the buf already when init buf!
		- we can change it if we find it
	- ! keyboard buffer != buf
		- used for CP5, 3 terminals spliting
		- buf: store the thing to show on the screen, read from user program
		- system call read!
			- terminal , polling process
		- user input -> key buf [terminal_read]-> buf1 [terminal_write]-> srcreen
	- scrolling : need help of paging!
	-   
- RTC
	- 1 people
	- rtc read != terminal read, it just useless ( not read rtc freq), one line is enougth to write it
		- due to simplification based on Linux
	- write: change freq
	- virtualize, still not need but useful in the future
		- hardware, more in ECE385
- Read Only File System Driver !!!
	- 2 people, hard 
	- [ ] lecture review
	-  