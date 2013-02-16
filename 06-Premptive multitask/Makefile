PATH := $(PATH):/opt/arm-sysmic-linux-uclibcgnueabi/bin

CC=arm-linux-gcc
CFLAGS=-Wall
AS=arm-linux-as
LD=arm-linux-ld
OBJ=arm-linux-objcopy
LDFLAGS=-N -Ttext=0x10000



os.bin : os.elf 
	arm-linux-objcopy -O binary os.elf os.bin

os.elf : linker.ld os.o startup.o context_switch.o syscall.o
	arm-linux-ld -N -Ttext=0x10000 -T linker.ld os.o startup.o syscall.o context_switch.o -o os.elf 

os.o : os.c include/timer.h include/VIC.h include/UART.h include/asm.h Task.h
	arm-linux-gcc -mcpu=arm926ej-s  -c os.c -o os.o
	
startup.o : startup.s
	arm-linux-as -mcpu=arm926ej-s -g startup.s -o startup.o
	
context_switch.o : context_switch.s
	arm-linux-as -mcpu=arm926ej-s -g context_switch.s -o context_switch.o

syscall.o : syscall.s
	arm-linux-as -mcpu=arm926ej-s -g syscall.s -o syscall.o
	
clean:
	rm -f *.o *.elf

