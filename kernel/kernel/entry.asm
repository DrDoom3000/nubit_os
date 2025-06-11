[bits 32]

extern kmain
global _start

section .text.entry

_start:
	sub esp,4
	jmp kmain
