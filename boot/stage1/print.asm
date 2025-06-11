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

