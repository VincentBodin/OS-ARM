OS
==

Different steps of the building of a multitask OS for an ARM target (ARM926)

Prerequisites : 
	- qemu for arm
	- an arm crosscompiler
	- arm-linux-gcc must be present in your PATH (done in the makefile)

How to launch the demos :
$ cd dir (where dir is the directory of the demo)
$ make
$ qemu-system-arm -M versatilepb -m 128M -nographic -kernel ./os.bin
