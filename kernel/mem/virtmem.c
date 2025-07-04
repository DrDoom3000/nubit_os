#define BLOCK_SIZE 4096
#define PAGE_TABLE 0xFFC00000
#define PAGE_DIRECTORY 0xFFFFF000
#define STACK 0xFFC00000
#define STACK_PHY 0x90000
#define VGA_TEXT 0xB8000
#define PAGE_SIZE (1<<12)

#include <stdint.h>
#include <stdbool.h>
#include "dadio.h"
#include "hal.h"
#include "phymem.h"
#include "virtmem.h"

static uint32_t* _page_directory = (uint32_t*)0x9C000;

enum PAGE_PTE_FLAGS {
	PTE_PRESENT		=	1,
	PTE_WRITABLE		=	2,
	PTE_USER		=	4,
	PTE_WRITETHOUGH		=	8,
	PTE_NOT_CACHEABLE	=	0x10,
	PTE_ACCESSED		=	0x20,
	PTE_DIRTY		=	0x40,
	PTE_PAT			=	0x80,
	PTE_CPU_GLOBAL		=	0x100,
	PTE_LV4_GLOBAL		=	0x200,
   	PTE_FRAME		=	0xFFFFF000 
};

enum PAGE_PDE_FLAGS {
 
	PDE_PRESENT		=	1,
	PDE_WRITABLE		=	2,
	PDE_USER		=	4,
	PDE_PWT			=	8,	
	PDE_PCD			=	0x10,
	PDE_ACCESSED		=	0x20,
	PDE_DIRTY		=	0x40,
	PDE_4MB			=	0x80,
	PDE_CPU_GLOBAL		=	0x100,
	PDE_LV4_GLOBAL		=	0x200,	
   	PDE_FRAME		=	0xFFFFF000 
};
static inline void entry_toggle_attrib (uint32_t* e, uint8_t attrib);
static inline void entry_set_frame (uint32_t*, uint32_t);
static inline bool entry_is_present (uint32_t e);
static inline bool entry_is_writable (uint32_t e);
static inline uint32_t entry_physical (uint32_t e);
static void set_recursive_map();
static bool alloc_page(uint32_t* table_entry);

extern uint32_t __begin[];
extern uint32_t __end[];
extern char  __VGA_text_memory[];
void vmmngr_init() 
{
	set_recursive_map();
	if(!map_page((uint32_t)__VGA_text_memory,VGA_TEXT))
		for(;;);
	if(!map_page(STACK-PAGE_SIZE,STACK_PHY-PAGE_SIZE))
		monitor_puts("Stack remap failed");
	if ((uint32_t)__end - (uint32_t)__begin > (2<<22))  
	{
		monitor_puts("Kernel spans more than 4M");
		for(;;);
	}
}

void remove_identity_map()  
{
	if (entry_is_present(_page_directory[0]))
		_page_directory[0] = 0;

	flush_tlb();  
}

bool map_page(uint32_t virtual_address,uint32_t physical_address)   
{
	if (virtual_address % BLOCK_SIZE) 
		virtual_address -= (virtual_address%BLOCK_SIZE);
	if (physical_address % BLOCK_SIZE) 
		physical_address -= (physical_address%BLOCK_SIZE);

	uint32_t pd_index = virtual_address >> 22;
	if (!entry_is_present(_page_directory[pd_index]))
	{
		if(!alloc_page(_page_directory+pd_index))
		       	return false;
		if(!entry_is_writable (_page_directory[pd_index]))
			entry_toggle_attrib (_page_directory + pd_index,PDE_WRITABLE);
	}
	uint32_t* page_table = (uint32_t*)(PAGE_TABLE | (pd_index<<12)); 
	uint32_t pt_index = ((virtual_address >> 12) & 0x3FF);

	entry_set_frame(page_table + pt_index,physical_address);  

	if(!entry_is_present (page_table[pt_index]))
		entry_toggle_attrib (page_table + pt_index,PTE_PRESENT);

	if(!entry_is_writable (page_table[pt_index]))
		entry_toggle_attrib (page_table + pt_index,PTE_WRITABLE);

	return true;
}
static void set_recursive_map()   
{
	uint32_t phy_dir = get_pdbr();
	uint32_t* vir_dir = _page_directory;
	entry_set_frame(vir_dir + 1023,phy_dir);
	if(!entry_is_present (vir_dir[1023]))
		entry_toggle_attrib (vir_dir + 1023,PDE_PRESENT);
	if(!entry_is_writable (vir_dir[1023]))
		entry_toggle_attrib (vir_dir + 1023,PDE_WRITABLE);
	_page_directory = (uint32_t*)PAGE_DIRECTORY; 
}
void free_page(uint32_t* table_entry)  
{
	if(!entry_is_present(*table_entry)) return;
	uint32_t physical_address = entry_physical (*table_entry);
	pmmngr_free_block( (uint32_t*) physical_address);
	entry_toggle_attrib (table_entry,PDE_PRESENT); 
}
static bool alloc_page(uint32_t* table_entry)   
{
	uint32_t* physical_address = pmmngr_allocate_block();
	if(!physical_address) return 0;

	entry_set_frame(table_entry, (uint32_t)physical_address);
	if(!entry_is_present (*table_entry))
		entry_toggle_attrib (table_entry,PDE_PRESENT);
	return 1;

}

static inline void entry_toggle_attrib (uint32_t* e, uint8_t attrib)  
{
	*e ^= attrib;
}

static inline void entry_set_frame (uint32_t* e, uint32_t physical_address)
{
	*e = (physical_address & PTE_FRAME)|( (*e) & (~PTE_FRAME) );
}

static inline bool entry_is_present (uint32_t e)
{
	return (e & PTE_PRESENT);

}
static inline bool entry_is_writable (uint32_t e)
{
	return (e & PTE_WRITABLE);
}


static inline uint32_t entry_physical (uint32_t e)  
{
	return (e & PTE_FRAME);
}

