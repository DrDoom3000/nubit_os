#include<stdint.h> 
#include<stdbool.h>
#include"phymem.h"
#include"dadio.h"

#define PMMAP 0x1000
#define KERNEL_P 0x100000
#define BLOCK_SIZE 4096
#define SECTOR_SIZE 512
#define SECTORS_PER_BLOCK 8
#define BLOCK_SIZE_B 12
#define SECTOR_SIZE_B 9
#define SECTORS_PER_BLOCK_B 3


#define KERNEL_MAPPED_SIZE 0x400000

extern uint32_t __begin[], __end[];

static void pmmngr_toggle_range(uint32_t start,uint32_t end);
static inline void pmmngr_toggle_block(uint32_t block_number);
static inline uint32_t block_number(uint32_t address);
static uint8_t get_lowest_bit(uint32_t hexinp);
static uint8_t extract_bit(uint32_t hexinp,uint8_t bitnumber); 


static uint32_t physical_memory_bitmap[0x8000];

typedef struct mmap_entry
{
    uint32_t  startLo;
    uint32_t  startHi;
    uint32_t  sizeLo;
    uint32_t  sizeHi;
    uint32_t  type;
    uint32_t  acpi_3_0;
} mmap_entry_t;


void  pmmngr_init(uint32_t mapentrycount)   
{ 
	mmap_entry_t* map_ptr= (mmap_entry_t*)PMMAP;


	for (uint32_t i=0;i<0x8000;i++)
	       	physical_memory_bitmap[i] = 0xffffffff; 

	for(uint32_t i=0;i<mapentrycount;i++)
	{
		if((map_ptr -> type == 1)&&(map_ptr -> startLo >= KERNEL_P) && !(map_ptr -> startHi))
			pmmngr_toggle_range(map_ptr->startLo, map_ptr->startLo + map_ptr ->sizeLo);
		map_ptr ++;
	}				
	pmmngr_toggle_range (KERNEL_P,KERNEL_P + KERNEL_MAPPED_SIZE);
}

uint32_t* pmmngr_allocate_block()
{
	uint32_t* address;
	for( uint32_t i=0;i<0x8000;i++)
		if (physical_memory_bitmap[i] < 0xffffffff)
		{
			uint8_t bit = get_lowest_bit(physical_memory_bitmap[i]);  
				if(bit == 0xff) return 0;
			address = (uint32_t*)((i << 17) + (bit << 12));
			pmmngr_toggle_block(block_number((uint32_t)address));
			return address;
		}
	return 0;
}
bool pmmngr_free_block(uint32_t* address)
{
	if((uint32_t)address % BLOCK_SIZE != 0) return 0;
	
	uint32_t block = block_number((uint32_t)address);
	uint32_t dword = block >> 5;
	uint8_t offset = block % 32;
	if(!extract_bit((uint32_t)(physical_memory_bitmap + dword),offset)) return 0;
	pmmngr_toggle_block(block);
	return 1;

}
uint32_t pmmngr_allocate_block_and_read() {
    uint32_t* address = pmmngr_allocate_block();
    if (!address) return 0;
    uint32_t value = *address;
    pmmngr_free_block(address);
    return value;
}
static uint8_t get_lowest_bit(uint32_t hexinp)
{
	for(int i=0;i<32;i++)
	{
		if ((hexinp%2) == 0)
			return i;
		hexinp >>= 1;
	}
	return 0xff;
}
static inline uint8_t extract_bit(uint32_t hexinp,uint8_t bitnumber)  
{
	return (hexinp >> bitnumber) & 1;
}
static inline uint32_t block_number(uint32_t address)
{
	return address >> BLOCK_SIZE_B;
}

static inline void pmmngr_toggle_block(uint32_t block_number)   
{
	physical_memory_bitmap[block_number >> 5] ^= (1ul << (block_number & 31));
}
/** start and end are addresses**/
static void pmmngr_toggle_range(uint32_t start,uint32_t end)    
{
	if (start % BLOCK_SIZE != 0){start -= (start%BLOCK_SIZE_B);}
	if (end % BLOCK_SIZE != 0){end -= (end%BLOCK_SIZE_B); end += BLOCK_SIZE;}
	while((end - start) >= BLOCK_SIZE)
	{
		
		if((end - start) >= 32* BLOCK_SIZE)
		{
			uint32_t* byte = (uint32_t*)(block_number(start) >> 3);
			byte = (uint32_t*)((uint8_t*)byte +(uint32_t)physical_memory_bitmap);
			*byte ^= 0xffffffff;
			start += (BLOCK_SIZE<<5);
			
		}
		else
		{
			pmmngr_toggle_block(block_number(start));
			start += BLOCK_SIZE;
		}
	}
}
