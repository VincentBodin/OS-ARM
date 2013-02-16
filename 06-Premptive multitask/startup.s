.text
.code 32

.global vectors_start
.global vectors_end

vectors_start:
	LDR PC, reset_handler_addr
	LDR PC, undef_handler_addr
	LDR PC, swi_handler_addr
	LDR PC, prefetch_abort_handler_addr
	LDR PC, data_abort_handler_addr
	B .
	LDR PC, irq_handler_addr
	LDR PC, fiq_handler_addr

reset_handler_addr: .word reset_handler
undef_handler_addr: .word undef_handler
swi_handler_addr: .word swi_handler
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr: .word data_abort_handler
irq_handler_addr: .word irq_handler
fiq_handler_addr: .word fiq_handler

vectors_end:

reset_handler:
	/* set Supervisor stack */
	LDR sp, =stack_top
	
	/* copy vector table to address 0 */
	BL copy_vectors
	
	/* get Program Status Register */
	MRS r0, cpsr /* Save the cpsr in r0 */
	
	/* go in IRQ mode */
	BIC r1, r0, #0x1F
	ORR r1, r1, #0x12
	MSR cpsr, r1
	
	/* set IRQ stack */
	LDR sp, =irq_stack_top
	
	/* Enable IRQs */
	BIC r0, r0, #0x80
	
	/* go back in Supervisor mode */
	MSR cpsr, r0
	
	/* jump to main */
	BL c_entry
	B .
	
irq_handler:
		
	SUB lr,lr,#4 
	STMFD sp!,{r0-r3,lr}

	msr CPSR_c, #0xDF /* System mode */
	STMFD sp!, {r0-r15}
	mov r0, sp
	/* Save current task context */
	BL SwitchTasks

	LDMFD sp!,{r0-r3,pc}^
	
swi_handler:

/* Save user state */
	msr CPSR_c, #0xDF /* System mode */
	/* Appeler une fonction de sauvegarde de contexte */
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov r0, sp
	msr CPSR_c, #0xD3 /* Supervisor mode */

	mrs ip, SPSR
	stmfd r0!, {ip,lr}

	/* Load kernel state */
	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr
	

.end
