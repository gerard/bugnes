#ifndef _SIMPLENES_PPU_H_
#define _SIMPLENES_PPU_H_

#include <stdint.h>

uint8_t *PPU_sprite_dma_dest();

uint8_t PPU_hook_cr1_read(uint16_t addr);
uint8_t PPU_hook_cr2_read(uint16_t addr);
uint8_t PPU_hook_status_read(uint16_t addr);
uint8_t PPU_hook_memdata_read(uint16_t addr);

uint8_t PPU_hook_cr1_write(uint16_t addr, uint8_t value);
uint8_t PPU_hook_cr2_write(uint16_t addr, uint8_t value);
uint8_t PPU_hook_memaddr_write(uint16_t addr, uint8_t value);
uint8_t PPU_hook_memdata_write(uint16_t addr, uint8_t value);

int PPU_load(int fd);

#endif
