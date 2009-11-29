#include <stdint.h>

/* This just holds the registers in static storage + setters/getters */
static uint8_t ppu_reg_CR1;
static uint8_t ppu_reg_CR2;
static uint8_t ppu_reg_SR;

/* Please check ppu_registers.h for rationale here */
uint8_t ppu_reg_i_CR1_get() { return ppu_reg_CR1; }
uint8_t ppu_reg_i_CR2_get() { return ppu_reg_CR2; }
uint8_t ppu_reg_i_SR_get()  { return ppu_reg_SR;  }

void ppu_reg_i_CR1_set(uint8_t v)   { ppu_reg_CR1 = v; }
void ppu_reg_i_CR2_set(uint8_t v)   { ppu_reg_CR2 = v; }
void ppu_reg_i_SR_set(uint8_t v)    { ppu_reg_SR  = v; }
