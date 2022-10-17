#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "idt.h"
#include "i8259.h"
#include "devices/keyboard.h"
#include "devices/RTC.h"

#define PASS 1
#define FAIL 0
#define KERNAL_START 0x400000
#define KERNAL_END	 0x800000
#define VIDEO_START  0xB8000
#define VIDEO_END	 0xB9000	 

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

int div_by_zero_test (){
	TEST_HEADER;

	int i = 10;
	int n = 0;
	int result = PASS;
	i = i / n;
	result = FAIL;
	return result;
}

int deref_null_pointer_test (){
	TEST_HEADER;
	//int i;
	//int a[3] = {0,0,0};
	int result = PASS;
	/*for (i = 0; i < 11; i++) {
		printf("%d", a[i]);
	}*/
	int *p = NULL;
	*p = 1;
	result = FAIL;
	//asm volatile ("INT $12");  
	return result;
}

int seg_not_present_test (){
	TEST_HEADER;
	int result = FAIL;
	asm volatile ("INT $45");  
	return result;
}

int test_other_exceptions (){
	TEST_HEADER;
	int result = FAIL;
	asm volatile ("INT $10");  
	return result;
}

int test_system_call (){
	TEST_HEADER;
	int result = PASS;
	asm volatile ("INT $0x80");  
	return result;
}

void i8259_disable_irq_garbege_test(){
	//send two invalid irq_num to the i8259
	// It shouldn't affect other device
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
			printf("Paging Directory Construct Successfully!\n");
		} else {
			printf("Paging Directory Construct Fail!\n");
		}
	} else {
		printf("Paging Directory Construct Fail!\n");
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
			printf("Paging Table Construct Successfully!\n");
		} else {
			printf("Paging Table Construct Fail!\n");
		}
	} else {
		printf("Paging Table Construct Fail!\n");
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
	printf("Paging Video Memory Success!\n");
	printf("Test location: %d", test);
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
	printf("Test location: %d", test);
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
	printf("Test location: %d", test);
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
	printf("Paging Kernal Sussess!\n");
	printf("Test location: %d", test);
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
	printf("Test location: %d", test);
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
	printf("Test location: %d", test);
	return result;
}
/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
// launch your tests here
void launch_tests(){
	printf("---------------TEST CP1 START--------------\n");	
	TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("div_by_zero_test", div_by_zero_test());
	// TEST_OUTPUT("deref_null_pointer_test", deref_null_pointer_test());
	// TEST_OUTPUT("seg_not_present_test", seg_not_present_test());
	// TEST_OUTPUT("test_other_exceptions", test_other_exceptions());
	// TEST_OUTPUT("test_system_call", test_system_call());
	/*Test for i8259*/
	i8259_disable_irq_garbege_test();	/*test whether garbage input will mask the interrupts*/
	i8259_disable_irq_test();			/*test whether disable_irq can mask the interrupts from keyboard and rtc*/
	i8259_enable_irq_garbege_test();	/*test whether garbage input will unmask the interrupts*/
	i8259_enable_irq_test();			/*test whether enable_irq can unmask the interrupts from keyboard and rtc*/

	// TEST_OUTPUT("page_dir_struct_test", page_dir_struct_test());
	// TEST_OUTPUT("page_table_struct_test", page_table_struct_test());
	// TEST_OUTPUT("page_test_video_mem_valid_test", page_test_video_mem_valid());
	// TEST_OUTPUT("page_test_video_mem_invalid", page_test_video_mem_invalid());
	// TEST_OUTPUT("page_test_video_mem_bottom_invalid", page_test_video_mem_bottom_invalid());
	// TEST_OUTPUT("page_test_kernal_valid", page_test_kernal_valid());
	// TEST_OUTPUT("page_test_kernal_invalid_top", page_test_kernal_invalid_top());
	// TEST_OUTPUT("page_test_kernal_invalid_bottom", page_test_kernal_invalid_bottom());
	printf("---------------TEST CP1 END--------------\n");
	
}
