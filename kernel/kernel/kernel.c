#include<stdint.h> 
#include "dadio.h"
#include "phymem.h"
#include "virtmem.h"
#include "timer.h"
#include "keyboard.h"
#include "hal.h"
#include "inthandling.h"
#include "ATA.h"
#include "FAT12.h"

extern void kshell();
extern void refresh_stack();
uint8_t array[512];
void initialize_all(uint32_t mmapsize);
void kmain(uint32_t mmapsize,uint32_t data_sect,uint32_t root_sect,uint32_t fat_sect)
{
	gdt_init();
	pmmngr_init(mmapsize); 
	vmmngr_init();  
	clear();

	refresh_stack(); 
	remove_identity_map();

	interrupt_init();
	kbc_init();
	set_timer(0xffff);

	kshell();

	clear_interrupts();
	kernel_wait();
}

