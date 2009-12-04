#include <stdint.h>
#include "ppu_registers.h"

/* This just holds the registers in static storage + setters/getters */
static uint8_t ppu_regs[PPU_REG_MAX];

static uint8_t calculate_mask(uint8_t shift, uint8_t size)
{
    return ((1 << size) - 1) << shift;
}

uint8_t ppu_reg_i_get(ppu_reg_t reg) {
    return ppu_regs[reg];
}

uint8_t ppu_reg_i_get_field(ppu_reg_t reg, uint8_t shift, uint8_t size)
{
    uint8_t mask = calculate_mask(shift, size);
    return (ppu_reg_i_get(reg) & mask) >> shift;
}


void ppu_reg_i_set(ppu_reg_t reg, uint8_t v)
{
    ppu_regs[reg] = v;
}

void ppu_reg_i_set_field(ppu_reg_t reg, uint8_t shift, uint8_t size, uint8_t val)
{
    uint8_t mask = calculate_mask(shift, size);
    uint8_t newval = (ppu_reg_i_get(reg) & ~mask) | (val << shift);

    assert((val << shift) <= mask);
    ppu_reg_i_set(reg, newval);
}
