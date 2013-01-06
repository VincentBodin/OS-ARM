#include "UART.h"
#include "asm.h"

void write_serial(char *s) {
	while(*s) {
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = *s;
		s++;
	}
}

void usertask(void) {
	write_serial("-usertask : First call of usertask !\n");
	write_serial("-usertask : Now, return to the kernell mode\n");
	syscall(); /* To return in the kernel's mode */
	write_serial("-usertask : Second call of usertask !\n");
	write_serial("-usertask : Now, return to the kernell mode\n");
	syscall(); /* To return in the kernel's mode */
	while(1);
}

int c_entry(void) {
	
	unsigned int usertask_stack[256];
	unsigned int *usertask_stack_start = usertask_stack + 256 - 16;
	usertask_stack_start[0] = 0x10;
	usertask_stack_start[1] = (unsigned int)&usertask;

	write_serial("OS : Starting...\n");
	write_serial("OS : Calling the usertask (1st time)\n");
	usertask_stack_start = activate(usertask_stack_start);
	write_serial("OS : Return to the OS mode !\n");
	write_serial("OS : Calling the usertask (2nd time)\n");
	usertask_stack_start = activate(usertask_stack_start);
	write_serial("OS : Return to the OS mode !\n");
	write_serial("OS : Going to infinite loop...\n");
	while(1); /* We can't exit, there's nowhere to go */
	return 0;
}
