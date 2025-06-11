[org 0x7c00]
[bits 16]

jmp near  boot_stage_1
bpbOEM			db "DummyBPB"
bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 	DB 2
bpbRootEntries: 	DW 512
bpbTotalSectors: 	DW 2880
bpbMedia: 		DB 0xf0  
bpbSectorsPerFAT: 	DW 6     ;We need all this
bpbSectorsPerTrack: 	DW 63
bpbHeadsPerCylinder: 	DW 16
bpbHiddenSectors: 	DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 		DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "DUMMY DISK "
bsFileSystem: 	        DB "FAT12   "
boot_stage_1:
cli
xor bx,bx
mov ss,bx
mov ds,bx
mov bp,0x9000
mov sp,bp
sti
mov [BootDiskNumber],dl
xor ax,ax
mov al,[bpbNumberOfFATs]
mul byte[bpbSectorsPerFAT] 
add ax,[bpbReservedSectors]
mov [ROOT_SECT_NO],ax
mov bx,[bpbRootEntries]
shr bx,4
add ax,bx
mov [DATA_SECT_NO],ax

call init_CHS   
LoadRoot:
	mov ax,[ROOT_SECT_NO]
	call LBA_to_CHS
	mov bx,[bpbRootEntries]
	shr bx,4   
	mov al,bl
	xor bx,bx
	mov es,bx
	mov bx,RootLoad  
	call disk_read_16
LoadFAT:
	mov ax,[bpbReservedSectors]
	call LBA_to_CHS
	mov al,[bpbSectorsPerFAT]
	mov bx,FATLoad  
	call disk_read_16
mov bx,Stage2Name
mov word[ReadWhere],Stage2Sector
call Findfile 
xor bx,bx
mov es,bx
mov bx,KernelName
mov word[ReadWhere],TempKernelSector
call Findfile 
END_OF_STAGE:
mov ah,0x00  
int 0x16

push word[bpbReservedSectors] 
push word[ROOT_SECT_NO] 
push word[DATA_SECT_NO] 

jmp (Stage2Sector << 4)
;Functions 
%include "boot/stage1/disk_read.asm"
%include "boot/stage1/print.asm"
%include "boot/stage1/CHS.asm"

Findfile:  ;Returns cluster number in bx
	mov si,.TargetMessage
	call print_si_16
	mov si,bx
	call print_si_16
	cld  
	mov di,RootLoad
	mov cx,[bpbRootEntries]  
	.Loop:
		push cx
		mov cx,11
		push di
		mov si,bx
		rep cmpsb
		pop di
		pop cx
		je .Found
		add di,0x20
	loop .Loop
	.NotFound 
		mov si, .Error
		call print_si_16
		jmp $

	.Error: db 0xa,0xd,'Not found!',0
	.Message: db 0xa,0xd,'Success!',0x0a,0x0d,0
	.TargetMessage: db 0xa,0xd,'Target:',0
	.Found:
		mov si,.Message
		call print_si_16
		add di,26
		mov dx,word[di]


ReadFile:
	xor cx,cx
	mov bx,[ReadWhere]
	mov es,bx
	xor bx,bx
	.loop:
	mov ax,dx   
	cmp ax,0xfff
	je .end
	sub ax,2
	add ax,[DATA_SECT_NO]
	
	push dx
	push cx
	call LBA_to_CHS
	mov al,1 
	call disk_read_16
	pop cx
	pop dx
	call GetNextSector
	add bx,512 
	inc cx
	jmp .loop
	.end:
	ret

GetNextSector:  ;dx is parameter
	push bx
	mov bx,dx 
	shl bx,1
	add bx,dx 
	shr bx,1   
	
	jc .odd
	.even:
	add bx,FATLoad
	mov dx,[bx]
	and dx,0x0fff
	pop bx 
	ret
	.odd:
	add bx,FATLoad
	mov dx,[bx]
	shr dx,4   
	pop bx
	ret
	

ROOT_SECT_NO: dw 0
DATA_SECT_NO: dw 0   
ReadWhere: dw 0
KernelName: db 'KERNEL  BIN',0
Stage2Name: db 'STAGE2  BIN',0
times 510 - ($-$$) db 0
dw 0xaa55


