#include "UART.h"
#include "asm.h"
#include "stddef.h"

#define STACK_SIZE 256 /* Size of our user task stacks in words */
#define TASK_LIMIT 2   /* Number of user task */

void write_serial(char *s) {
	while(*s) {
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = *s;
		s++;
	}
}

/* Function that creates user task */
unsigned int *init_task(unsigned int *stack, void (*start)(void)) {
	stack += STACK_SIZE - 16; /* End of stack, minus what we're about to push */
	stack[0] = 0x10; /* User mode, interrupts on */
	stack[1] = (unsigned int)start;
	return stack;
}

void userFunction(void) {
	write_serial("-usertask : First call of usertask !\n");
	write_serial("-usertask : Now, return to the kernell mode\n");
	syscall();
	write_serial("-usertask : Second call of usertask !\n");
	while(1) syscall(); /* To return in the kernel's mode */
}

void anotherUserFunction(void) {
	
	while(1){
		write_serial("-anotherusertask : Call of anotherUserFunction !\n");
		write_serial("-anotherusertask : Now, return to the kernell mode\n"); 
		syscall(); /* To return in the kernel's mode */
	}
}

int c_entry(void) {
	
	//unsigned int user_stacks[2][256]; /* Two staks to run two tasks */
	//unsigned int *usertask[2]; /* Our two tasks */
	
	///* Implementation of our tasks */
	//usertask[0] = user_stacks[0] + 256 - 16;
	//usertask[0][0] = 0x10;
	//usertask[0][1] = (unsigned int)&userFunction;

	//usertask[1] = user_stacks[1] + 256 - 16;
	//usertask[1][0] = 0x10;
	//usertask[1][1] = (unsigned int)&anotherUserFunction;
	
	unsigned int user_stacks[TASK_LIMIT][STACK_SIZE];
	unsigned int *usertasks[TASK_LIMIT];
	size_t task_count = 0;
	size_t current_task = 0;

	usertasks[0] = init_task(user_stacks[0], &userFunction);
	task_count += 1;	
	usertasks[1] = init_task(user_stacks[1], &anotherUserFunction);
	task_count += 1;
	
	write_serial("OS : Starting...\n");
	write_serial("OS : Scheduler implementation : Round Robin\n");
	while(1) {
		usertasks[current_task] = activate(usertasks[current_task]);
		current_task++;
		if(current_task >= task_count) current_task = 0;
	}
	
	
	return 0;
}
