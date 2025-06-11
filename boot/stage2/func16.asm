[bits 16]
print_si_16:
        pusha 
        mov ah,0x0e 
        print_16_loop: 
                lodsb 
                cmp al,0 
                je print_16_end 
                int 0x10 
                jmp print_16_loop 
        print_16_end: 
        popa 
        ret

print_hex_bx:
	pusha
	mov ah,0x0e
	mov al,'0'
	int 0x10
	mov al,'x'
	int 0x10
	mov cx,4
	.loop:
		mov ax,bx
		and ax,0xf000
		shr ax,12
		cmp ax,9
		jg .print_hex_letter
		add ax,0x30  
		jmp .print_hex_conv_end
		.print_hex_letter:
		add ax,55  

		.print_hex_conv_end:
			mov ah,0x0e  
			int 0x10

		shl bx,4
	loop .loop
	popa
	ret

get_cursor_16:
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
