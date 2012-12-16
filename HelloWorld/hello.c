#define UART0 ((volatile unsigned int*)0x101F1000) // Adresse de UART0 PrimeCell PL011
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
	print_uart0("Hello world!\n");
}
