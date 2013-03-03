#include "include/UART.h"
#include "include/asm.h"
#include "include/timer.h"
#include "include/VIC.h"
#include "stddef.h"
//#include "Task.h"

#define STACK_SIZE 256 /* Size of our user task stacks in words */
#define TASK_LIMIT 2   /* Number of user task */
#define RUN			0
#define READY		1
#define WAIT		2




typedef struct
{
	unsigned int *sp;
	unsigned int registers[12];
	unsigned int lr;
	unsigned int pc;
	unsigned int cpsr;
	unsigned int mode;
	unsigned int num;
	int stacksize;

	int priority;
	int state;                    /* Running, Ready, Waiting, Suspended */

	/* Next and previous task in the queue */
	struct taskstruct *qnext, *qprev;
}taskstruct;


taskstruct task[3];
unsigned int task_count;
unsigned int currentTask;


void print_uart0(char *s) {
	while(*s) {
		while(UARTFR & UARTFR_TXFF);
		UART0 = *s;
		s++;
	}
}


init_task(taskstruct * task, unsigned int* stack, void (*function)(void) ){



	stack += STACK_SIZE;// - 16; /* End of stack, minus what we're about to push */
	//stack[0] = 0x10; /* User mode, interrupts on */
	//stack[1] = (unsigned int)function;

	task->sp = stack;

	task->sp[0] = task->registers[0]; // r0
	task->sp[1] = task->registers[1];// = task->sp[3];//0; // r1
	task->sp[2] = task->registers[2];// = task->sp[4];//0; // r2
	task->sp[3] = task->registers[3];// = task->sp[5];//0; // r3
	task->sp[4] = task->registers[4];// = task->sp[6];//0; // r4
	task->sp[5] = task->registers[5];// = task->sp[7];//0; // r5
	task->sp[6] = task->registers[6];// = task->sp[8];//0; // r6
	task->sp[7] = task->registers[7];// = task->sp[9];//0; // r7
	task->sp[8] = task->registers[8];// = task->sp[10];//0; // r8
	task->sp[9] = task->registers[9];// = task->sp[11];//0; // r9
	task->sp[10] = task->registers[10];// = task->sp[12];//0; // r10
	task->sp[11] = task->registers[11];// = task->sp[13];//0; // r11
	task->sp[12] = task->registers[12];// = task->sp[14];//0; // r12
	task->sp[13] = (unsigned int)function;

	/*task->pc = task->sp[13];
	task->lr = task->pc;

	task->cpsr = 0;
	task->mode = 0x10;*/
}


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



/* Debug function */
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


void c_entry(void) {
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
	/*init_task(&task[2],&task3Function);*/

	// Link tasks
/*	task[0].qnext = &task[1];
	task[1].qnext = &task[0];
	task[2].qnext = &task[3];
	task[3].qnext = &task[0];*/

	displayWelcomeMessage();
	print_uart0("OS : Starting...\n");
	print_uart0("OS : Scheduler implementation : Round Robin\n");


	// Timer1 Configuration
	TIMER01_disable();
	TIMER01_LOAD_VALUE = 65535;
	//TIMER01_CONTROL |= 0x00000008;
	TIMER01_enable();


	activate(task[currentTask].sp);

	while(1){
		/* Disable IRQ interrupt */
		TIMER01_disable();
		print_uart0("Kernel gets back control ! \n");
		print_uart0("Load the next task ! \n");
		currentTask = currentTask+1;
		if(currentTask >= task_count) currentTask = 0;
		TIMER01_LOAD_VALUE = 65535;
		TIMER01_enable();
		activate(task[currentTask].sp);
	}
}


//void __attribute__((interrupt)) irq_handler() {
void event_irq_handler(void){
 /* Determine the interrupt's source */

int src_IRQ = VIC_BASE_ADDR;
int i = 0;

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
TIMER01_LOAD_VALUE = 50000;
//TIMER01_enable();

}


void saveTaskContext(int * ptr){
	int i = 0;

	/* UPDATE THE STACK TASK */
	for ( i = 0 ; i <= 13 ; i++ ){
		task[currentTask].sp[i] = *(ptr+i);
	}

	/*task[0].sp[13] = *(ptr+13); // LR
	task[0].sp[14] = task[0].sp[13];// Cause it's where we have to branch the next time

	print_uart0("SAVED REGISTERS : \n");

	for (i = 0 ; i <= 13 ; i ++ ){
		printable = task[0].sp[i] + (int)'0';
		print_uart0(&printable);
		print_uart0("\r\n");
	}*/
}


int * loadTaskContext( int* ptr){

	int i = 0;
	char printable;

	for( i = 0 ; i <= 12 ; i++)
		*(ptr+i) = task[currentTask].registers[i];

	 *(ptr+14) =task[currentTask].lr;

	return ptr;
}

void event_swi_handler(int taskNumber){

	unsigned char c_taskNumber = ((taskNumber & 0x000F) +(unsigned int)'0');
	print_uart0("task Number that called the syscall : ");
	print_uart0(&c_taskNumber);
	print_uart0("\r\n");
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


