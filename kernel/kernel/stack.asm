STACK equ 0xFFC00000
STACK_PHY equ 0x90000

global refresh_stack

refresh_stack:
	mov ebx,[esp]
	mov esp,STACK
	mov ebp,STACK
	jmp ebx

