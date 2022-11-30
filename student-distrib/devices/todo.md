# TODO LIST

## Keyboard
1. define list 
// Used for CP2

#define CAPSLOCK_PRESSED 0x3A
#define CAPSLOCK_RELEASED 0xBA
#define ALT_PRESSED 0x38
#define ALT_RELEASED 0xB8
#define F1  0x3B
#define F2  0x3C
#define F3  0x3D
#define F4  0x3E
#define F5  0x3F
#define ESC 0x01
#define TAB_PRESSED 0x0F
#define UP_PRESSED 0x48
#define DOWN_PRESSED 0X50

2. clear the screen function
                if (ctrl_buf == 1 && value == 'l'){
                    clear();
                    printf("\n\n\n clear the screen\n");
                    // sti();
                    return;
                }

    it's cool but hard to realize?
    I need to clearify the knowledge of screen buf in our OS

3. improve the scancode
   
      {0,0},
    {0,0},    
    {'1','!'},
    {'2','3','4','5','6','7','8','9','0','-','=',0,
    0,'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';',39, '`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,0,0,
    '\0'},
    {}
- sol: improve as a 58*2 col structure like ZLH's, because it will be more efficient


3. update_cursor
finished, but page fault
i think it is because scolling unimplement

4. scolling
    - memory lift up for one line, 
    - do we need to care lift up?
5. terminal structure
    - [x] must implement now 
    - [x] in terminal.h
    - [x] where to initialize it?
    - [x] construct test 
    - [x] construct open lines

## Q&A

4. how to test the extreme input case of keyboard

Answer: think of what extreme cases there could be. What happens when you type all 128 characters? Does the backspace button break things when you keep pressing even when there's nothing left? Does the capslock affect number keys when it shouldn't?

5. how to handle backspace, can you give me some hint?

Answer: You would have to modify putc in lib.c. You would need to move the screen_x and screen_y values (move the cursor backwards), print a space character to erase the previous character. And update keyboard buffer accordingly.

6. Can you provide an example, why should we keep track of the screen position?

Answer: putc already uses screen_x and screen_y. e.g. when you call putc on a character and screen_x is at the right most cell, you need to set screen_x to be 0 and screen_y++ to print to a new line.

7. what is the relationship between present terminal_write and future CP5?
Answer:  good question, useful
do it clearly so future work will be easy
but how??
maybe I can read the document on 3 terimals now


- explain more after the cp5

8. is the cursor unrealized, I mean, we need to realize its moving? the blinking _ after every input
Answer: yes, and it's easy



@ Peizhe Liu: When terminal read is called, it will keep looping until there is a keyboard interrupt that sends a signal that \n is pressed and it will stop that while loop and start copying from keyboard buffer to the buffer provided to the terminal read argument


I am confused about the buffers in keyboard and terminal. So calling terminal read will copy from keyboard buffer and wait for a new line character? What will happen to keyboard buffer if terminal read is not called and a new line character is inputted?

@ Peizhe Liu. The keyboard handler will handle putting each key in the keyboard buffer, and calling putc(character) to print that character. You need to modify putc to handle scrolling and what happens when you call putc('\b') and putc('\t') for backspace and tab.

Thank you. So when terminal read is not called, we don't care the existing characters? Only start putting them to the buffer if terminal read is called, and return when a new line character is putted.


