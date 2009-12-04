#ifndef _HOOKS_H_
#define _HOOKS_H_

/* This file contains all those hooks which are not implemented in the PPU */

#define PPU_CR1_MAP                 0x2000
#define PPU_CR2_MAP                 0x2001
#define PPU_STATUS_MAP              0x2002
#define PPU_SPRITE_ADDR_MAP         0x2003
#define PPU_SPRITE_DATA_MAP         0x2004
#define PPU_SCR_SCOLL_MAP           0x2005
#define PPU_MEM_ADDR_MAP            0x2006
#define PPU_MEM_DATA_MAP            0x2007
#define PPU_SPRITE_DMA_MAP          0x4014

#define INTERNAL_RAM_MIRROR_START   0x0000
#define INTERNAL_RAM_MIRROR_STOP    0x2000
#define INTERNAL_RAM_MIRROR_SIZE    0x0800
uint16_t mirror(uint16_t addr, uint16_t start, uint16_t size);
uint16_t mirror_internal_ram(uint16_t addr);

#endif
