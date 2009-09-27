#include <stdio.h>
#include <assert.h>
#include "memhooks.h"
#include "core.h"

/* MODULE VARIABLES: CPU State and registers */
static uint16_t reg_pc;
static uint8_t short_regs[REG_MAX];

/* MODULE VARIABLES: Memory and Stack */
/* The stack takes 256B in the 0x0100-0x01FF address space */
static uint8_t mem[0x10000];
static uint8_t *stack = &mem[0x100];

/**
 * MEMORY
 */
uint8_t MEM_READ(uint16_t addr)
{
    memhook_fun_t mh_f;
    mh_f = memhook_check(MEMHOOK_TYPE_READ, addr);

    return mh_f ? mh_f(addr, NULL) : mem[addr];
}

/* Note that non-aligned memory access is fine */
uint16_t MEM_READ16(uint16_t addr)
{
    return (MEM_READ(addr+1) << 8) + MEM_READ(addr);
}

void MEM_WRITE(uint16_t addr, uint8_t v)
{
    memhook_fun_t mh_f;
    mh_f = memhook_check(MEMHOOK_TYPE_WRITE, addr);

    if (!mh_f) mem[addr] = v;
    else mh_f(addr, &mem[addr]);
}

void MEM_WRITE16(uint16_t addr, uint16_t v)
{
    mem[addr] = v & 0xFF;
    mem[addr+1] = ((v & 0xFF00) >> 8);
}

void MEM_INC(uint16_t addr, int8_t v)
{
    /* We don't support this to simplify things for now */
    assert(!memhook_check(MEMHOOK_TYPE_WRITE, addr));
    mem[addr] += v;
}

/**
 * STACK
 */
uint8_t STACK_POP()
{
    return stack[++short_regs[REG_SP]];
}

uint16_t STACK_POP16()
{
    uint8_t lsb = STACK_POP();
    uint8_t msb = STACK_POP();

    return (msb << 8) + lsb;
}

void STACK_PUSH(uint8_t v)
{
    stack[short_regs[REG_SP]--] = v;
}

void STACK_PUSH16(uint16_t v)
{
    uint8_t msb = (v & 0xFF00) >> 8;
    uint8_t lsb = v & 0xFF;

    STACK_PUSH(msb);
    STACK_PUSH(lsb);
}


/**
 * REGISTERS
 * Note that flag specific (REG_CPU) macros are given in flags.h
 */
/* Register getters */
uint8_t GET_SREG(short_reg_t reg)               { return reg != REG_INVALID ? short_regs[reg] : 0; }
uint16_t GET_PC()                               { return reg_pc; }

/* Registers setters */
void SET_SREG(short_reg_t reg, uint8_t v)       { if (reg != REG_INVALID) short_regs[reg] = v; }
void SET_PC(uint16_t v)                         { reg_pc = v; }
void SET_PC_COND(uint16_t v, uint8_t condition) { if (condition) SET_PC(v); }

/**
 * FLAGS
 */
int GET_FLAG(uint8_t flag)                      { return !!(short_regs[REG_CPU] & flag); }
void SET_FLAG(uint8_t flag, int v)              { short_regs[REG_CPU] = v ? short_regs[REG_CPU] | flag : short_regs[REG_CPU] & ~flag; }
