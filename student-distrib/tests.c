#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "idt.h"
#include "i8259.h"
#include "devices/keyboard.h"
#include "devices/RTC.h"
#include "devices/terminal.h"
#include "file_system.h"
#include "syscall.h"

#include "game/MentOS.h"

#define PASS 1
#define FAIL 0
#define KERNAL_START 0x400000
#define KERNAL_END	 0x800000
#define VIDEO_START  0xB8000
#define VIDEO_END	 0xB9000
#define RTC_DATA_BYTES		4
#define RTC_TEST_VALUS		3


/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

// void file_system_init(uint32_t* fs_start);
// int file_open(const uint8_t* fname);
// uint32_t file_read(int32_t fd, uint8_t* buf, int32_t nbytes);
// int file_write();
// int file_close(int32_t fd);
// int dir_open();
// uint32_t dir_read(int32_t fd, uint8_t* buf, int32_t nbytes);
// int dir_write();
// int dir_close();
// void files_ls();


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 20; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

/* div_by_zero_test()
 * Inputs: none
 * Outputs: divide_by_0 excpetions
 * Return Value: FAIL
 * Function: test divide by 0 exception
 * if exception works, the function prints
 * the exception output and freezes
 * otherwise, the function completes without
 * exception and the test is failed
 */

int div_by_zero_test (){
	TEST_HEADER;

	int i = 10;
	int n = 0;
	int result;
	// divide by 0 exception invoked here
	i = i / n;
	// the test is failed at this stage
	result = FAIL;
	return result;
}

/* deref_null_pointer_test()
 * Inputs: none
 * Outputs: page fault excpetions
 * Return Value: FAIL
 * Function: the function dereferences
 * a null pointer, which will cause a 
 * page fault. If the function can 
 * successfully complete, it will return
 * a test FAIL
 */

int deref_null_pointer_test (){
	TEST_HEADER;
	//int i;
	//int a[3] = {0,0,0};
	int result = PASS;
	/*for (i = 0; i < 11; i++) {
		printf("%d", a[i]);
	}*/
	// dereferencing a null pointer
	// will cause a page fault
	int *p = NULL;
	*p = 1;

	// the test is failed at this stage
	result = FAIL;
	return result;
}

/* seg_not_present_test()
 * Inputs: none
 * Outputs: segmentation not presented excpetions
 * Return Value: FAIL
 * Function: the function will int a function
 * at idt 45, which supposed to be empty for now
 * If the function successfully returns, the test
 * fails.
 */

int seg_not_present_test (){
	TEST_HEADER;
	int result = FAIL;
	// call IDT[45], which is not presented
	asm volatile ("INT $45");  
	return result;
}


/* test_other_exceptions()
 * Inputs: none
 * Outputs: invalid TSS excpetions
 * Return Value: FAIL
 * Function: the function calls any exception
 * in the IDT just to see if anything shows up.
 * Returns fail if no exception
 */
int test_other_exceptions (){
	TEST_HEADER;
	int result = FAIL;
	// call IDT[10], which is not TSS invalid
	asm volatile ("INT $10");  
	return result;
}

/* test_system_call()
 * Inputs: none
 * Outputs: system call
 * Return Value: FAIL
 * Function: the function calls system calls
 */

int test_system_call (){
	TEST_HEADER;
	int result = PASS;
	asm volatile ("INT $0x80");  // system call at 0x80
	return result;
}

void i8259_disable_irq_garbege_test(){
	//send two invalid irq_num to the i8259
	// It shouldn't affect other device
	printf(" ENTERING\n");
	disable_irq(0x10);
	disable_irq(666);

	
}
void i8259_enable_irq_garbege_test(){
	//send two invalid irq_num to the i8259
	// It shouldn't affect other device
	enable_irq(0x10);
	enable_irq(666);
	
}
void i8259_disable_irq_test(){
	// Simply close the devices
	disable_irq(KEYBOARD_IRQ);
	disable_irq(RTC_IRQ);
}

void i8259_enable_irq_test(){
	// Simply open the devices
	enable_irq(KEYBOARD_IRQ);
	enable_irq(RTC_IRQ);
}


/****** PAGING TESTS ******/

int page_dir_struct_test() {
	TEST_HEADER;
	int result = FAIL;
	clear();
	if (page_directory[0].pd_kb.present == 1 && page_directory[1].pd_mb.present == 1) {
		if (page_directory[3].pd_kb.present == 0) { // 3 change be change by from 2 - 1023 (the no present page directory entry)
			result = PASS;
			printf(" Paging Directory Construct Successfully!\n");
		} else {
			printf(" Paging Directory Construct Fail!\n");
		}
	} else {
		printf(" Paging Directory Construct Fail!\n");
	}
	
	return result;
}

int page_table_struct_test() {
	TEST_HEADER;
	int result = FAIL;
	clear();
	if (page_table[0].present == 0 && page_table[5].present == 0) { // index can be 0-1023 rather than xb8;
		if (page_table[VIDEO_MEMORY >> PT_SHIFT].present == 1) { // 3 change be change by from 2 - 1023 (the no present page directory entry)
			result = PASS;
			printf(" Paging Table Construct Successfully!\n");
		} else {
			printf(" Paging Table Construct Fail!\n");
		}
	} else {
		printf(" Paging Table Construct Fail!\n");
	}
	return result;
}

/* page_test_video_mem_valid()
 * Inputs: none
 * Return Value: test result
 * Function: test the case of dereferencing a valid video memory location 
 */
int page_test_video_mem_valid() {
	TEST_HEADER;
	int result = PASS;
	uint32_t *ptr = (uint32_t *) VIDEO_START;
	int test = *ptr;
	clear();
	printf(" Paging Video Memory Success!\n");
	printf(" Test location: %d", test);
	return result;
}

/* page_test_video_mem_invalid()
 * Inputs: none
 * Return Value: test result
 * Function: test the case of dereferencing the invalid video memory location
 * 			 above video memory
 */
int page_test_video_mem_invalid() {
	TEST_HEADER;
	int result = PASS;
	uint32_t *ptr = (uint32_t *) VIDEO_START-1;
	int test = *ptr;
	clear();
	printf(" Test location: %d", test);
	return result;
}

/* page_test_video_mem_bottom_invalid()
 * Inputs: none
 * Return Value: test result
 * Function: test the case of dereferencing the invalid video memory location
 *			 below video memory
 */
int page_test_video_mem_bottom_invalid() {
	TEST_HEADER;
	clear();
	int result = PASS;
	uint32_t *ptr = (uint32_t *) VIDEO_END;
	int test = *ptr;
	printf(" Test location: %d", test);
	return result;
}

/* page_test_kernal_valid()
 * Inputs: none
 * Return Value: test result
 * Function: test the case of dereferencing a valid kernal location */
int page_test_kernal_valid() {
	TEST_HEADER;
	int result = PASS;
	uint32_t *ptr = (uint32_t *) KERNAL_START;
	int test = *ptr;
	clear();
	printf(" Paging Kernal Sussess!\n");
	printf(" Test location: %d", test);
	return result;
}

/* page_test_kernal_invalid_top()
 * Inputs: none
 * Return Value: test result
 * Function: test the case of dereferencing an invalid kernal location*/
int page_test_kernal_invalid_top() {
	TEST_HEADER;
	int result = PASS;
	uint32_t *ptr = (uint32_t *) KERNAL_START-1;
	int test = *ptr;
	clear();
	printf(" Test location: %d", test);
	return result;
}

/* page_test_kernal_invalid_bottom()
 * Inputs: none
 * Return Value: test result
 * Function: test the case of dereferencing a valid kernal location */
int page_test_kernal_invalid_bottom() {
	TEST_HEADER;
	int result = PASS;
	uint32_t *ptr = (uint32_t *) KERNAL_END;
	int test = *ptr;
	clear();
	printf(" Test location: %d", test);
	return result;
}

/* rtc_set_freq_test()
 * Inputs: none
 * Return Value: none
 * Function: test change of the freq of the rtc
 */
void rtc_set_freq_test() {
	TEST_HEADER;
	rtc_set_freq(8); // the number can be change as 2^(form 1 to 15)
}

/* Checkpoint 2 tests */

/****** Terminal TESTS ******/
//CP2:
void fully_functional_keyboard_test(){
	// int terminal_mode = 1; // The control boolean to set the termianl output mode

	printf("\n\n\n=== Keyboard Test===\n");
	printf("0. There is another fully functional terminal test\n");
	printf("1. For keyboard input, there is two kind of ideas on the input limit \n");
	printf("2. And we designed both, selected by a control unit terminal_mode:\n");
	printf("     -- terminal_mode == 0, unlimited keyboard input \n");
	printf("     -- terminal_mode == 1, limited to 127 keyboard input \n");
	printf("     -- Defaultly set as 1\n");
	printf("3. Please enter any value within the standard 58 keyboard scanset below:\n");

}
void terminal_test(){
	
	const int8_t test_str[] = "DID MP3 ANNOYS YOU? SIGH!\n";
	clear();
	printf("=====Start testing the terminal open/read/write/close driver=====\n");
	int32_t fd = 3; // Testing, set fd as any number
	// uint32_t test1,
	int test2;
	int cnt;
	uint8_t buf[1000];
	// uint8_t write_buffer[] = "Test the terminal write\n"; // its length is 24
	memset(buf,NULL,KEY_BUF_SIZE);
	const uint8_t filename[10] = "TEST.txt";
	// Open
	printf("[Test terminal_open()]$\n");
	if( 0 != terminal_open(filename)){
		printf("Can't open terminal here\n");
	}else{
		printf("---Pass the open test\n");
	}
	// Close
	printf("[Test terminal_close()]$\n");
	if( 0 != terminal_close(fd)){
		printf("Can't open terminal here\n");
	}else{
		printf("---Pass the open test\n");
	}

	// TEST WRITE FROM A NORMAL BUF
	printf("[Test terminal_write()]$\n");
	printf("terminal output is: ");
	if (0 != terminal_write(fd,(void*)test_str,27)){
		printf("---Pass the write test\n");
	}else{
		printf("Fail to pass the write test.\n");
	}

	// Loop to test the input case
	printf("[TEST terminal_read() and write()]$\n");
	printf("Start testing terminal-keyboard interrupt\n");
	while (1){
		
		printf("[user@localhost]$ ");
		printf("Hi, what's your name? ");

		if (-1 == (cnt = terminal_read (0, buf, 1000-1))) {
        printf ("Can't read name from keyboard.\n");
		}else{
			printf("The number of bytes is read: %d\n", cnt);

		}
		buf[cnt] = '\0';
    	terminal_write (1, "Hello, ",strlen("Hello, "));
    	test2 = terminal_write (1, buf,1000);
		printf("The number of bytes is write: %d\n", test2);

    }
	
}




// /* File System Tests */

// /* filesys_ls_test()
//  * Inputs: none
//  * Return Value: none
//  * Function: test file list function
//  */
// void filesys_ls_test() {
// 	clear();
//     dir_open();
//     files_ls();
// }

/* filesys_frame0_test()
 * Inputs: none
 * Return Value: none
 * Function: test displaying frame0 of fish
 */
void filesys_frame0_test() {
	uint8_t temp_buf[LARGE_BUF_SIZE];
	clear();
    //file_open((uint8_t *) "frame0.txt");
    file_read(2, temp_buf, LARGE_BUF_SIZE);
}

/* filesys_cat_test()
 * Inputs: none
 * Return Value: none
 * Function: test displaying executable cat
 */
void filesys_cat_test() {
	uint8_t temp_buf[LARGE_BUF_SIZE];
	clear();
    //((uint8_t *) "cat");
    file_read(2, temp_buf, LARGE_BUF_SIZE);
}

/* filesys_long_name_fail_test()
 * Inputs: none
 * Return Value: none
 * Function: test opening a file with name exceeding 32B; should fail
 */
void filesys_long_name_fail_test() {
	uint8_t temp_buf[LARGE_BUF_SIZE];
	clear();
    //file_open((uint8_t *)  "verylargetextwithverylongname.txt");
    file_read((uint32_t) 2, temp_buf, LARGE_BUF_SIZE);
}

/* filesys_long_name_success_test()
 * Inputs: none
 * Return Value: none
 * Function: test reading a file with exactly 32B in name
 */
void filesys_long_name_success_test() {
	uint8_t temp_buf[LARGE_BUF_SIZE];
	clear();
    //file_open((uint8_t *)  "verylargetextwithverylongname.tx");
    file_read((uint32_t) 2, temp_buf, (uint32_t) LONG_FILE_SIZE);
}

/* filesys_file_open_failed_test()
 * Inputs: none
 * Return Value: none
 * Function: test reading a non-existing file with a really long name
 */
void filesys_file_open_failed_test(){
	clear();
	//file_open((uint8_t *)  "thisisasuperultrareallyreallylongfilename");
}

/* filesys_file_read_half_test()
 * Inputs: none
 * Return Value: none
 * Function: test reading half of a text file
 */
void filesys_file_read_half_test(){
	uint8_t temp_buf[LARGE_BUF_SIZE];
	clear();
	//file_open((uint8_t *) "frame0.txt");
	file_read((uint32_t) 0, temp_buf, (uint32_t) HALF_FRAME0);
}

// /* filesys_dir_read_test()
//  * Inputs: none
//  * Return Value: none
//  * Function: test dir_read that read file names one by one in the directory
//  */
// void filesys_dir_read_test(){
// 	clear();
// 	uint32_t idx;
// 	uint8_t temp_buf[LARGE_BUF_SIZE];
// 	dir_open();
// 	for (idx = 0; idx < FILE_COUNT; idx++){
// 		dir_read((uint32_t) 0, temp_buf, (uint32_t) FILENAME_LEN);
// 		printf("file name: %s\n", temp_buf);
// 	}
// }


//  RTC driver test 

/* //  RTC rtc_open_read_close_test test ()
 * Inputs: none
 * Return Value: test result
 * Function: test the open, read, close for rtc
 */
// int rtc_open_read_close_test() {
// 	TEST_HEADER;
// 	int result = PASS;
// 	// give the test value
// 	int test = RTC_TEST_VALUS;
// 	uint8_t filename = RTC_TEST_VALUS;
// 	uint32_t fd = RTC_TEST_VALUS;
// 	uint32_t buf = RTC_TEST_VALUS;
// 	clear();
// 	rtc_init();
// 	// test open
// 	printf("Wait for RTC opens......\n");
// 	test = rtc_open(&filename);
// 	if (test != RTC_SUCCESS) {
// 		return FAIL;
// 	}
// 	// test read
// 	printf("RTC_read begins\n");
// 	printf("Wait for RTC interrupt occurs......\n");
// 	rtc_read(fd, &buf, RTC_DATA_BYTES);
// 	// test close
// 	printf("Wait for RTC closes......\n");
// 	test = rtc_close(fd);
// 	if (test != RTC_SUCCESS) {
// 		return FAIL;
// 	}

// 	return result;
// }

/* rtc_write_test()
 * Inputs: none
 * Return Value: test result
 * Function: test the write for rtc
 */
// int rtc_write_test() {
// 	TEST_HEADER;
// 	int result = PASS;
// 	int test = RTC_TEST_VALUS;
// 	uint8_t filename = RTC_TEST_VALUS;
// 	uint32_t fd = RTC_TEST_VALUS;
// 	uint32_t frequency = RTC_OPEN_DEFAULT_FREQ;
// 	clear();
// 	rtc_init();
// 	// the parameters of the open,read and close are not used in the function during the CP2
// 	printf("Wait for RTC opens......\n");
// 	test = rtc_open(&filename);
// 	if (test != RTC_SUCCESS) {
// 		return FAIL;
// 	}
// 	printf("current frequency is %d Hz\n", frequency);
// 	// using a counter to change the rate
// 	while (frequency <= RTC_INIT_DEFAULT_FREQ) { // <= 1024 //TODO: the range of rtc frequency?
// 		// for each frequency, there are total 32 times rtc interrupt
// 		if (rtc_counter >= RTC_TEST_COUNTER) {
// 			// change to the next ferquency
// 			clear();
// 			// go to the next reasonable frequency
// 			frequency *= 2;
// 			if (frequency <= RTC_INIT_DEFAULT_FREQ) {
// 				printf("current frequency is %d Hz\n", frequency);
// 			}
// 			// pass the new frequency into the rtc
// 			rtc_write(fd, &frequency, RTC_DATA_BYTES);
// 			// reset the counter
// 			rtc_counter = 0;
// 		}
// 	}
// 	// after test, reset the frequency back to the 2 Hz
// 	frequency = RTC_OPEN_DEFAULT_FREQ;
// 	rtc_write(fd, &frequency, RTC_DATA_BYTES);

// 	printf("Wait for RTC closes......\n");
// 	rtc_close(fd);
// 	test = rtc_close(fd);
// 	if (test != RTC_SUCCESS) {
// 		return FAIL;
// 	}

// 	return result;
// }

/* rtc_valid_input_frequency_test(uint32_t freq)
 * Inputs: uint32_t freq -- the input frequency
 * Return Value: test result
 * Function: test the invalid input frequency for rtc
 */
// int rtc_invalid_input_frequency_test(uint32_t freq) {
// 	TEST_HEADER;
// 	int result = PASS;
// 	// give the test values
// 	int test = RTC_TEST_VALUS;
// 	uint8_t filename = RTC_TEST_VALUS;
// 	uint32_t fd = RTC_TEST_VALUS;
// 	uint32_t current_freq = RTC_OPEN_DEFAULT_FREQ;
// 	clear();
// 	rtc_init();
// 	// the parameters of the open,read and close are not used in the function during the CP2
// 	printf("Wait for RTC opens......\n");
// 	test = rtc_open(&filename);
// 	if (test != RTC_SUCCESS) {
// 		return FAIL;
// 	}

// 	printf("current frequency is %d Hz\n", current_freq);
// 	// write the invalid frequency into the rtc
// 	rtc_write(fd, &freq, RTC_DATA_BYTES);

// 	printf("Wait for RTC closes......\n");
// 	rtc_close(fd);
// 	test = rtc_close(fd);
// 	if (test != RTC_SUCCESS) {
// 		return FAIL;
// 	}

// 	return result;
// }
/* Checkpoint 3 tests */

/* test_syscall_linkage
 * - this test function is used for early test 
 * - Since we improves pcb from local static to dynamic generated,
 * - This test function can't be used anymore
 * - Please don't try to use it!
 */
void test_syscall_linkage() {
	asm volatile (
	"movl $0x01,%eax 		;"
	"INT $0x80				;"
	);
} 
/* test_sys_open
 * - this test function is used for early test 
 * - Since we improves pcb from local static to dynamic generated,
 * - This test function can't be used anymore
 * - Please don't try to use it!
 */
void test_sys_open() {
	printf("start testing open\n");
	// int ret;
	// sys_execute((uint8_t*)"testprint");
	// ret = sys_open (".");
	// ret = sys_open ("cat");
	// ret = sys_open ("fsdir");
	// ret = sys_open ("rtc");
	// ret = sys_open("frame0.txt");

	// uint8_t temp_buf[LARGE_BUF_SIZE];
	// sys_read(2, temp_buf, LARGE_BUF_SIZE);
}


/* execute_test
 * - test function for Checkpoint 3
 * - Current "Main" program
 */
void execute_test() {
	clear();
	//UI_START();
	printf("Welcome to MentOS!\n");
	sys_execute((uint8_t*)"shell");
	/*Inidivitual test*/
	// sys_execute((uint8_t*)"testprint");
	// sys_execute((uint8_t*)"hello ");
	// sys_execute((uint8_t*)"pingpong ");
	// sys_execute((uint8_t*)"fish ");
	// sys_execute((uint8_t*)"shell ");


}



/* Checkpoint 4 tests */
// int vidmap_test(){
// 	TEST_HEADER;
// 	int result = PASS;
// 	int correct_PT = 1 + 2 + 4 + 16 + 64 +(0xB8000 >> 12);
// 	printf("correctPT is %d\n",correct_PT);
// 	uint8_t test_addr[5];
// 	test_addr[0] = (uint8_t)0x08300000;
// 	int ret = sys_vidmap(test_addr[0]); // TODO : how to input the argument?
// 	if (0 !=  ret)
// 	{
// 		result = FAIL;
// 	}
// 	if( correct_PT != vid_page_table[0].val){
// 		result = FAIL;
// 	}
// 	printf("vidPT is %d\n",vid_page_table[0].val);

// 	return result;
// }
/*
    vid_page_table[0].present = 1;
    vid_page_table[0].read_write = 1;
    vid_page_table[0].user_supervisor = 1;  
    vid_page_table[0].base_addr = VIDEO_MEMORY >> PT_SHIFT;        
    vid_page_table[0].cache_disabled = 1;
    vid_page_table[0].dirty = 1;

*/
/* Checkpoint 5 tests */

/* CP5: 
 * - main program for checkpoint 5
 */
void CP5() {
	clear();
	//UI_START();
	printf("Welcome to MentOS!\n");
	terminal_init();

	sys_execute((uint8_t*)"shell");



}

/* Test suite entry point */
// launch your tests here
void launch_tests(){
	// Let us start MentOS
	CP5();
	/***** CP3,4 TESTS *****/
	//printf("---------------TEST CP3,4 START--------------\n");
	// Here we delete those out of used test, leaving the excute along
	// vidmap_test();
	// printf("---------------TEST CP3,4 END--------------\n");


	/***** CP2 TESTS *****/
	//printf("---------------TEST CP2 START--------------\n");	
	// terminal_mode = 1; // The control boolean to set the termianl output mode
	// execute_test();
	/*Test for rtc_driver*/
	// TEST_OUTPUT("rtc_open_read_close_test", rtc_open_read_close_test());
	// TEST_OUTPUT("rtc_write_test", rtc_write_test());
	// TEST_OUTPUT("rtc_invalid_input_frequency_test", rtc_invalid_input_frequency_test(3));

	/* File System Tests */
	// filesys_ls_test();			// list all files and their file types and sizes
	// filesys_frame0_test();		// read a normal text file
	// filesys_cat_test();			// read an executable file
	// filesys_long_name_fail_test();	// reading a file with name exceeding 32B; should fail
	// filesys_long_name_success_test();	// reading a file with name exactly 32B; should succeed
	// filesys_file_open_failed_test();		// read a non-existent file; should fail
	// filesys_file_read_half_test();		// read half of a file
	// filesys_dir_read_test();				// test directory read

	/* Terminal Tests */
	// fully_functional_keyboard_test();
	// terminal_test();


	// printf("---------------TEST CP2 END--------------\n");




	// printf("---------------TEST CP1 START--------------\n");	
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("div_by_zero_test", div_by_zero_test());
	// TEST_OUTPUT("deref_null_pointer_test", deref_null_pointer_test());
	// TEST_OUTPUT("seg_not_present_test", seg_not_present_test());
	// TEST_OUTPUT("test_other_exceptions", test_other_exceptions());
	// TEST_OUTPUT("test_system_call", test_system_call());
	// /*Test for i8259*/
	// i8259_disable_irq_garbege_test();	/*test whether garbage input will mask the interrupts*/
	// i8259_disable_irq_test();			/*test whether disable_irq can mask the interrupts from keyboard and rtc*/
	// i8259_enable_irq_garbege_test();	/*test whether garbage input will unmask the interrupts*/
	// i8259_enable_irq_test();			/*test whether enable_irq can unmask the interrupts from keyboard and rtc*/

	/*Test for rtc set frequency*/
	// rtc_set_freq_test();

	/*Test for Paging */
	// TEST_OUTPUT("page_dir_struct_test", page_dir_struct_test());
	// TEST_OUTPUT("page_table_struct_test", page_table_struct_test());
	// TEST_OUTPUT("page_test_video_mem_valid_test", page_test_video_mem_valid());
	// TEST_OUTPUT("page_test_video_mem_invalid", page_test_video_mem_invalid());
	// TEST_OUTPUT("page_test_video_mem_bottom_invalid", page_test_video_mem_bottom_invalid());
	// TEST_OUTPUT("page_test_kernal_valid", page_test_kernal_valid());
	// TEST_OUTPUT("page_test_kernal_invalid_top", page_test_kernal_invalid_top());
	// TEST_OUTPUT("page_test_kernal_invalid_bottom", page_test_kernal_invalid_bottom());
	// printf("---------------TEST CP1 END--------------\n");
	
}

