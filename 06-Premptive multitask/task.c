#include "include/task.h"
#include "os.h"

void init_task(taskstruct * task, unsigned int* stack, void (*function)(void) ){

	task->sp = stack;

	task->sp[0] = task->registers[0]; // r0
	task->sp[1] = task->registers[1]; // r1
	task->sp[2] = task->registers[2]; // r2
	task->sp[3] = task->registers[3]; // r3
	task->sp[4] = task->registers[4]; // r4
	task->sp[5] = task->registers[5]; // r5
	task->sp[6] = task->registers[6]; // r6
	task->sp[7] = task->registers[7]; // r7
	task->sp[8] = task->registers[8]; // r8
	task->sp[9] = task->registers[9]; // r9
	task->sp[10] = task->registers[10]; // r10
	task->sp[11] = task->registers[11]; // r11
	task->sp[12] = task->registers[12]; // r12
	task->sp[13] = (unsigned int)function; // lr

}
