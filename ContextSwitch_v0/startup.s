.global _Reset
_Reset:
	LDR sp, =0x07FFFFFF
	BL c_entry
	B .
