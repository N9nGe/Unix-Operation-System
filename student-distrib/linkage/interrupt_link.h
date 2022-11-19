/* interrupt_link.h - Assembly linkage for device and idt
 * vim:ts=4 noexpandtab
 */
#ifndef _INTERRUPT_LINK_H_
#define _INTERRUPT_LINK_H_

void rtc_handler_linkage();
void keyboard_handler_linkage();
//CP5
void pit_handler_linkage();
#endif
