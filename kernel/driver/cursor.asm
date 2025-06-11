[bits 32] 
[global set_cursor]
[global get_cursor]

set_cursor:   ;bx is the location

	mov ecx,[esp+0x4]
	mov al, 0x0f
	mov dx, 0x3d4
	out dx,al
	mov dx,0x3d5
	mov al,cl 
	out dx,al

	mov al, 0x0e
	mov dx, 0x3d4
	out dx,al
	mov dx,0x3d5
	mov al,ch
	out dx,al

	ret

get_cursor:

	mov al, 0x0f
	mov dx, 0x3d4
	out dx,al
	mov dx,0x3d5
	in al,dx
	mov bl,al

	mov al, 0x0e
	mov dx, 0x3d4
	out dx,al
	mov dx,0x3d5
	in al,dx
	mov bh,al

	movzx eax,bx
	
	
	ret

