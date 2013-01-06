#include <stdint.h>

#define UART0 ((volatile unsigned int*)0x101f1000) // Adresse de UART0 PrimeCell PL011
#define UARTFR 0x06 // Offset pour pointer le registre UARTFR (Flag register)
#define UARTFR_TXFF 0x20 // Masque à appliquer pour lire l'état du flag buffer d'émission
// bit 3  : Busy - bit 5 : Transmission Fifo Full

#define VIC_BASE_ADDR  ((volatile unsigned int *)0x10140000) /* IRQ STATUS REGISTER -> shows the active interrupt */
#define VIC_INT_SELECT ((volatile unsigned int *)0x1014000C) //offset = 0x00C Select if interrupt is IRQ(0) or FIQ(1)
#define VIC_ENABLE_INT ((volatile unsigned int *)0x10140010) //offset = 0x010 Enables the selected interrupt
#define VIC_CLEAR_INT  ((volatile unsigned int *)0x10140014) //offset 0x014 Clear the selected interrupt

#define TIMER01_LOAD_VALUE ((volatile unsigned int *)0x101E2000) // Writes the value directly in TIMER0
#define TIMER01_CURRENT_VALUE ((volatile unsigned int *)0x101E2004) // Current Value
#define TIMER01_CONTROL ((volatile unsigned char *)0x101E2008) // Control Register
#define TIMER01_CLEAR_INT  ((volatile unsigned char *)0x101E200C) // Clear interrupt whatever we write into
#define TIMER01_BGLOAD_VALUE  ((volatile unsigned int *)0x101E2018) // Writes the value in TIMER0 after it reaches 0
#define TIMER01_enable() ((*TIMER01_CONTROL | 0x00000080))
#define TIMER01_disable() ((*TIMER01_CONTROL & (~0x00000080)))


void print_uart0(const char *s) {
	while(*s != '\0') 
	{ /* Loop until end of string */
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = (unsigned int)(*s); /* Transmit char */
		s++; /* Next char */
	}
}
	 
void c_entry() {
	print_uart0("Hello world!\n");
	
	// VIC Configuration
	*VIC_INT_SELECT = 0; // All interrupts are IRQ
	*VIC_ENABLE_INT = 0x00000080; // Enable Timer0 Interrupt
	*VIC_CLEAR_INT =  0x00000080; // Clear potential occured interrupt
	
	// Timer1 Configuration
	TIMER01_disable();
	*TIMER01_LOAD_VALUE = 65535;
	TIMER01_enable();
	
	while(1);
}

void __attribute__((interrupt)) irq_handler() {
 /* Determine the interrupt's source */
 print_uart0("Interruption raised");
	switch(*VIC_BASE_ADDR)
	{
		case 0x08 : print_uart0("Interruption from timer 0");
		default : print_uart0("Interruption not form timer 0");
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
