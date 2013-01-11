#include "timer.h"
#include "VIC.h"

#define UART0 ((volatile unsigned int*)0x101f1000) // Adresse de UART0 PrimeCell PL011
#define UARTFR 0x06 // Offset pour pointer le registre UARTFR (Flag register)
#define UARTFR_TXFF 0x20 // Masque à appliquer pour lire l'état du flag buffer d'émission
// bit 3  : Busy - bit 5 : Transmission Fifo Full



void print_uart0(const char *s) {
	while(*s != '\0') 
	{ /* Loop until end of string */
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = (unsigned int)(*s); /* Transmit char */
		s++; /* Next char */
	}
}
	

void c_entry() {
	
	char timerValue[32];
	int i =0;
	print_uart0("Hello world!\n");
	
	
	// VIC Configuration
	*VIC_INT_SELECT = 0; // All interrupts are IRQ
	*VIC_ENABLE_INT = 0x00000010; // Enable Timer01 Interrupt
	//*VIC_CLEAR_INT =  0x00000010; // Clear potential occured interrupt
	
	// Timer1 Configuration
	TIMER01_disable();
	TIMER01_LOAD_VALUE = 65520;
	TIMER01_enable();
	
	while(1){
		//~ //for(i=0; i<31;i++){
			//~ timerValue[0] = '0'+ (*TIMER01_CURRENT_VALUE);
			//~ print_uart0(&timerValue[0]);
		//~ //}
		//~ print_uart0("\r\n");
		
	} 
}

void __attribute__((interrupt)) irq_handler() {
 /* Determine the interrupt's source */
 print_uart0("Interruption raised\r\n");
	switch(*VIC_BASE_ADDR)
	{
		case 0x00000010 : print_uart0("Interruption from timer 0\r\n");
							break;
		default : print_uart0("Interruption not form timer 0\r\n");
	}
}

/* all other handlers are infinite loops (for the moment) */
void __attribute__((interrupt)) undef_handler(void) { for(;;); }
void __attribute__((interrupt)) swi_handler(void) { for(;;); }
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
