struct taskstruct
{

	void * sp;
	unsigned int registers[12];
	unsigned int lr;
	unsigned int pc;
	unsigned int cpsr;
	unsigned int mode;
	unsigned int num;
	void *stack;
	int stacksize;

	int priority;
	int state;                    /* Running, Ready, Waiting, Suspended */

	/* Next and previous task in the queue */
	struct taskstruct *qnext, *qprev;
};

//void init_task(struct taskstruct* task, void * function);
