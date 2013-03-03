#include "include/UART.h"
#include "include/asm.h"
#include "include/timer.h"
#include "include/VIC.h"
#include "include/task.h"
#include "os.h"
#include "stddef.h"

/****************************************************************************************/
/********* USER TASK SECTION *************/
#define STACK_SIZE 256 /* Size of our user task stacks in words */
#define TASK_LIMIT 3   /* Number of user task */

void task1Function(void) {
	print_uart0("-usertask : First task is started...\r\n");
	while(1){
		print_uart0("-usertask : First task is running...\r\n");
	}
}

void task2Function(void) {
		print_uart0("-usertask : Second task is started...\r\n");
		//syscall(); /* To return in the kernel's mode */
		while(1){
			print_uart0("-usertask : Second task is running...\r\n");
		}
}

void task3Function(void) {
		print_uart0("-usertask : Third task is started...\r\n");
		syscall(3); /* To return in the kernel's mode */
}
/***************************************************************************************/

void print_uart0(char *s) {
	while(*s) {
		while(UARTFR & UARTFR_TXFF);
		UART0 = *s;
		s++;
	}
}

unsigned int task_count;
unsigned int currentTask;
taskstruct task[TASK_LIMIT];

void c_entry(void) {
	int i = 0;
	task_count = 0;
	currentTask = 0;

	/** VIC Configuration **/
	VIC_INT_SELECT = 0; // All interrupts are IRQ
	VIC_ENABLE_INT = 0x00000210; // Enable Timer01 Interrupt and UART0



	unsigned int user_stacks[TASK_LIMIT][STACK_SIZE];

	// Task initialization
	init_task(&task[0], user_stacks[0], &task1Function);
	task_count = task_count + 1;
	init_task(&task[1], user_stacks[1], &task2Function);
	task_count = task_count + 1;
	init_task(&task[2], user_stacks[2], &task3Function);
	task_count = task_count + 1;


	//displayWelcomeMessage();
	print_uart0("OS : Starting...\n");
	print_uart0("OS : Scheduler implementation : Round Robin\n");


	// Timer1 Configuration
	TIMER01_disable();
	TIMER01_LOAD_VALUE = 65535;
	TIMER01_enable();


	activate(task[currentTask].sp);

	while( i < 5 ){
		TIMER01_disable();
		print_uart0("Kernel gets back control ! \n");
		print_uart0("Kernel can do some stuff...\n");
		print_uart0("Load the next task ! \n");

		/* Scheduler */
		currentTask = currentTask+1;
		if(currentTask >= 2) currentTask = 0; // We only start the first and second task

		TIMER01_LOAD_VALUE = 65535;
		TIMER01_enable();

		activate(task[currentTask].sp);
		TIMER01_disable();
		i = i + 1;
	}

	print_uart0("Kernel's going to activate the third task which will call a syscall() to return in Kernel mode \n");
	activate(task[2].sp);
	print_uart0("Kernel gets back control ! \n");
	print_uart0("Now, the OS will be shutdown, press CTRL+a,x to stop QEMU");

	while(1); // Nowhere to go
}




/**********************************************************/
/*** 				HANDLERs SECTION	 				***/
/**********************************************************/

void event_irq_handler(void){

int src_IRQ = VIC_BASE_ADDR;

/* Disable all interrupts and clear all flags */
TIMER01_disable();
TIMER01_CLEAR_INT = 1;
VIC_CLEAR_INT = 0xFFFFFFFF;


 print_uart0("\n**********************************\n");
 print_uart0("\tInterruption raised\n");
 print_uart0("************************************\n");

	switch(src_IRQ & 0x00000010)
	{
		case 0x00000010 : 	print_uart0("Interruption from timer 0\t\n");
							break;
		case 0x00000800 :	print_uart0("Interruption from UART0\t\n");
							break;
		default : print_uart0("Interruption unknown\r\n");
	}

VIC_ENABLE_INT = 0x00000010; // Enable Timer01 Interrupt
}

void event_swi_handler(int taskNumber){

	char printable = taskNumber +(int)'0';
	print_uart0("Task number that called the syscall : ");
	print_uart0(&printable);
	print_uart0("\r\n");
}
/**************************************************************************************/

void saveTaskContext(int * ptr){
	int i = 0;

	/* UPDATE THE STACK TASK */
	for ( i = 0 ; i <= 13 ; i++ ){
		task[currentTask].sp[i] = *(ptr+i);
	}

}


/* all unimplemented handlers are infinite loops (for the moment) */
void __attribute__((interrupt)) undef_handler(void) { for(;;); }
void __attribute__((interrupt)) prefetch_abort_handler(void) { for(;;); }
void __attribute__((interrupt)) data_abort_handler(void) { for(;;); }
void __attribute__((interrupt)) fiq_handler(void) { for(;;); }


/* Copy the vector table to the 0x00000000 adddress */
void copy_vectors(void)
 {
	extern unsigned int vectors_start;
	extern unsigned int vectors_end;
	unsigned int *vectors_src = &vectors_start;
	unsigned int *vectors_dst = (unsigned int *)0;

	while(vectors_src < &vectors_end)
		*vectors_dst++ = *vectors_src++;
}





/**********************************************************/
/*** 				NOT USED FUNCTIONS !! 				***/
/**********************************************************/
afficheValeurRegistres( int * ptr)
{
	char printable;
		int * pointeur;
		int i;
		print_uart0("Affichage des registres reçus\r\n");

		for( i = 1; i <= 4; i++){
			printable = *(ptr+i) + (int)'0';
			print_uart0(&printable);
			print_uart0("\r\n");
		}
}

int * loadTaskContext( int* ptr){

	int i = 0;
	char printable;

	for( i = 0 ; i <= 12 ; i++)
		*(ptr+i) = task[currentTask].registers[i];

	 *(ptr+14) =task[currentTask].lr;

	return ptr;
}

