[bits 16]
BootDiskNumber: db 0
disk_read_16:
	pusha
		mov dl,[BootDiskNumber]
		mov ah,0x02 ;interrupt function
		int 0x13
	popa
	ret
