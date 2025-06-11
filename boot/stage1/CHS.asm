init_CHS:
pusha
	mov dl,0x80
	xor bx,bx
	mov di,bx
	mov ah , 0x08
	int 0x13
	inc dh
	mov [HEADS],dh
	mov bl,cl
	and bl,0x3f
	mov [SPT],bl
	mov bl,ch
	mov ch,0
	shr cx,6
	mov bh,cl
	inc bx
	mov [CYLINDER],bx
popa
ret

HEADS: db 0
SPT:db 0
CYLINDER: dw 0

LBA_to_CHS:
push ax
push bx
	xor bx,bx
	
	xor dx,dx
	mov bl,[SPT]
	div bx
	inc dx
	mov cl,dl
	and cl,0x3f

	xor dx,dx
	mov bl,[HEADS]
	div bx
	shl dx,8
	mov ch,al
pop bx
pop ax
ret
