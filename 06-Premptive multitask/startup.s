.text
.code 32

.global vectors_start
.global vectors_end

.EQU USR_MODE, 0x10
.EQU SYS_MODE, 0x1F
.EQU SVC_MODE, 0x13
.EQU IRQ_MODE, 0x12
.EQU INT_OFF, 0xC0


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
	MOV sp, #0x80000
	
	MSR CPSR_c, #INT_OFF|SYS_MODE
	MOV sp, #0x40000
	
	MSR CPSR_c, #INT_OFF|IRQ_MODE
	MOV sp, #0x10000
	
	MSR CPSR_c,#INT_OFF|SVC_MODE
	
	/* Copy table vector to address 0 */
	BL copy_vectors

	/* jump to main */
	B c_entry
	
irq_handler:
	/* Save the return value */
	SUB lr,lr,#4
	BL event_irq_handler


	MOV r12,lr /* r12 contains the return value of usertask*/
	MSR CPSR_c, #INT_OFF|SYS_MODE
	MOV lr, r12
	STMFD sp!,{r0-r12,lr}
	NOP
	/*MOV r0, sp
	BL afficheValeurRegistres*/

	MOV r0, sp
	BL saveTaskContext

	/*BL loadTaskContext
	NOP
	LDR r1, [r0, #4]
	LDR r2, [r0, #8]
	LDR r3, [r0, #12]
	LDR r4, [r0, #16]
	LDR r5, [r0, #20]
	LDR r6, [r0, #24]
	LDR r7, [r0, #28]
	LDR r8, [r0, #32]
	LDR r9, [r0, #36]
	LDR r10, [r0, #40]
	LDR r11, [r0, #44]
	LDR r12, [r0, #48]
	/*LDR sp [r0, #52]  JUMP OVER SP */
	/* LDR lr, [r0, #56] NOT NEEDED */
	/*LDR pc, [r0, #56]*/


	/*MSR CPSR_c, #INT_OFF|IRQ_MODE*/
	/*LDMFD sp!,{r0-r3,r12,pc}^*/

	/* Load kernel state */
	MSR CPSR_c,SVC_MODE
	LDMFD sp!,{r1-r11,pc}
	NOP
	
swi_handler:

	/* Save user state */
	MSR CPSR_c, #INT_OFF|SYS_MODE
	STMFD sp!,{r0-r12,lr}
	NOP
	MOV r0, sp
	BL saveTaskContext

	NOP
	/* Load kernel state */
	MSR CPSR_c,SVC_MODE
	LDMFD sp!,{r0-r11,pc}
	NOP
	

.end
