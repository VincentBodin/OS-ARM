#include "Task.h"

void init_task(struct taskstruct* task, void * function ){
	task->sp = &function;
	task->registers[0] = 0; // r0
	task->registers[1] = 0; // r1
	task->registers[2] = 0; // r2
	task->registers[3] = 0; // r3
	task->registers[4] = 0; // r4
	task->registers[5] = 0; // r5
	task->registers[6] = 0; // r6
	task->registers[7] = 0; // r7
	task->registers[8] = 0; // r8
	task->registers[9] = 0; // r9
	task->registers[10] = 0; // r10
	task->registers[11] = 0; // r11
	task->registers[12] = 0; // r12
	task->lr = 0;
	task->pc = 0;
	task->cpsr = 0;
	task->mode = 0x10;
}

