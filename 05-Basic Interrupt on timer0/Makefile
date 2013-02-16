PATH := $(PATH):/opt/arm-sysmic-linux-uclibcgnueabi/bin


hello.bin : hello.elf hello.o startup.o
	arm-linux-objcopy -O binary hello.elf hello.bin

hello.elf : linker.ld hello.o startup.o
	arm-linux-ld -T linker.ld hello.o startup.o -o hello.elf -N -Ttext=0x10000

hello.o : hello.c
	arm-linux-gcc -mcpu=arm926ej-s -c hello.c -o hello.o

startup.o : startup.s
	arm-linux-as -mcpu=arm926ej-s -g startup.s -o startup.o


