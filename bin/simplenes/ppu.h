#ifndef _SIMPLENES_PPU_H_
#define _SIMPLENES_PPU_H_

uint8_t PPU_cr1_read(uint16_t addr);
uint8_t PPU_cr2_read(uint16_t addr);
uint8_t PPU_status_read(uint16_t addr);
uint8_t PPU_memdata_read(uint16_t addr);

uint8_t PPU_cr1_write(uint16_t addr, uint8_t value);
uint8_t PPU_cr2_write(uint16_t addr, uint8_t value);
uint8_t PPU_memaddr_write(uint16_t addr, uint8_t value);
uint8_t PPU_memdata_write(uint16_t addr, uint8_t value);

int PPU_load(int fd);

#endif
