#ifndef phymem_h
#define phymem_h

#include <stdint.h>
#include <stdbool.h>
void  pmmngr_init(uint32_t mapentrycount);
uint32_t* pmmngr_allocate_block();
bool pmmngr_free_block(uint32_t* address);
uint32_t pmmngr_allocate_block_and_read();
#endif
