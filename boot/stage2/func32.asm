[bits 32]  
;global print_esi_32
;global set_cursor_32
;global clrscr_32

print_esi_32:   
	pusha
	call get_cursor_32
	shl ebx,1   
	add ebx,0xb8000 

	_print_pm_loop:
		mov al,[esi]
		mov [ebx],ax
		add esi,1
		add ebx,2
		cmp al,0
		je _print_pm_end
	jmp _print_pm_loop
	_print_pm_end:
	sub ebx,0xb8000
	shr ebx,1   
	dec ebx
	call set_cursor_32

	popa
	ret

set_cursor_32:   
	push dx
	push ax
	mov al, 0x0f 
	mov dx, 0x3d4 
	out dx,al   
	mov dx,0x3d5 
	mov al,bl 
	out dx,al

	mov al, 0x0e 
	mov dx, 0x3d4 
	out dx,al
	mov dx,0x3d5 
	mov al,bh
	out dx,al

	pop ax
	pop dx
	ret

get_cursor_32:
	push dx
	push ax
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
	
	pop ax
	pop dx
	ret

clrscr_32:
	pusha
	mov ebx,0xb8000
	mov al,' '
	mov cx,2000 
	clrscr_loop:
		mov word[ebx],ax
		add ebx,2
	loop clrscr_loop
	mov bx,0
	call set_cursor_32
	popa
	ret
