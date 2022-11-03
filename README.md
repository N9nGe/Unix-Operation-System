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



2. ## 3.3 Overview: System Calling: MP3 Checkpoint 3

Q1. What is the usage of Syscall folder outside the student-distrib?
Prof's Ans: It's empty architecture prepared to help you 

Paging: Start to think in terms of x86 instructions !

CP3: all are shell instructions!

execute(hardest):
halt ~= RETURN, stop the program process
	- never return?
	- delete everything
open,close, read,write

CP4: rest 4

System call has its convention!

- EAX: # of call
- EBX: 1st arg
- ECX: 2nd arg
- EDX: 3rd arg
- Return value -> EAX


file position: offset, a book mark

MMU: Memory Management Unit
https://zh.wikipedia.org/wiki/%E5%86%85%E5%AD%98%E7%AE%A1%E7%90%86%E5%8D%95%E5%85%83
V.A. -> [CPU | MMU] -> P.A.
Flush TLB: if not, security problems, can't access other 
TLB: done by hardware, but need to take care when and where to flush it
https://zh.wikipedia.org/wiki/%E8%BD%89%E8%AD%AF%E5%BE%8C%E5%82%99%E7%B7%A9%E8%A1%9D%E5%8D%80

3. 3.2 Overview

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
	- [x] lecture review
	-  