#include "core.h"
#include "addr.h"

/* 6502 CPUs do the increments before jumping, thats why A_REL (which is
 * a 2 bytes addressing mode) jumps to PC+2+(immediate).  The effective
 * range is -126, 129
 * Ref: http://www.obelisk.demon.co.uk
 */
uint16_t ADDR(addr_mode_t mode)
{
    uint16_t addr = MEM_READ16(GET_PC() + 1);
    uint8_t addr_zero_page = MEM_READ(GET_PC() + 1);
    int8_t relative = (int8_t)MEM_READ(GET_PC() + 1);

    switch (mode) {
    case A_IMM: return GET_PC() + 1;
    case A_ZPA: return addr_zero_page;
    case A_ZPX: return addr_zero_page + GET_SREG(REG_X);
    case A_ZPY: return addr_zero_page + GET_SREG(REG_Y);
    case A_REL: return GET_PC() + 2 + relative;
    case A_ABS: return addr;
    case A_ABX: return addr + GET_SREG(REG_X);
    case A_ABY: return addr + GET_SREG(REG_Y);
    case A_IND: return MEM_READ16(addr);
    case A_INX: return MEM_READ16(addr_zero_page + GET_SREG(REG_X));
    case A_INY: return MEM_READ16(addr_zero_page) + GET_SREG(REG_Y);
    default:    return INVALID_ADDRESS;
    }
}

/* Pesky ACC addressing mode... */
uint8_t VALUE(addr_mode_t mode)
{
    switch (mode) {
    case A_ACC: return GET_SREG(REG_ACC);
    default:    return 0;
    }
}

int ADDR_VALID(uint16_t addr)
{
    return addr != INVALID_ADDRESS;
}

uint8_t ADDR_MODE_GET_VALUE(addr_mode_t mode)
{
    return ADDR_VALID(ADDR(mode)) ? MEM_READ(ADDR(mode)) : VALUE(mode);
}

