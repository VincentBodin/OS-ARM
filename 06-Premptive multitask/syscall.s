.global syscall
syscall:
	/* Here, r0 contains the task number */
	swi 0
	MOV lr, pc
	bx lr
