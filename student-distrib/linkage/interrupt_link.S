# interrupt_link.h - Assambly linkage for device and idt
 
#define INTR_LINK(name, func)       \
    .GLOBL name                    ;\
    name:                          ;\
        pushal                     ;\
        pushfl                     ;\
        call func                  ;\
        popfl                      ;\
        popal                      ;\
        iret

INTR_LINK (keyboard_handler_linkage, keyboard_interrupt_handler);
INTR_LINK (rtc_handler_linkage, rtc_interrupt);
INTR_LINK (pit_handler_linkage, pit_interrupt_handler);

