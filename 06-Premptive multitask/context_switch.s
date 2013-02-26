.EQU USR_MODE, 0x10
.EQU SYS_MODE, 0x1F
.EQU SVC_MODE, 0x13
.EQU IRQ_MODE, 0x12
.EQU INT_OFF, 0xC0


.global activate
activate:



	/*LDR r12, [r0]*/
	STMFD sp!,{r1-r11,lr}
	NOP
	msr CPSR_c, #0x10 /* User mode with IRQ enabled and FIQ disabled*/
	/*mov sp, r0*/
	mov pc, r1

	/*pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	msr CPSR_c, #0xD3 /* Supervisor mode */

	/*movs pc, lr*/
