#ifndef _CORE_H_
#define _CORE_H_

#include <stdint.h>

/* TODO: Figure out proper order for the flags */
#define FLAG_C      (1 << 0)
#define FLAG_Z      (1 << 1)
#define FLAG_I      (1 << 2)
#define FLAG_D      (1 << 3)
#define FLAG_B      (1 << 4)
#define FLAG_V      (1 << 6)
#define FLAG_N      (1 << 7)

#define CPU_ADDR_RESERVED_START     0xFFFA
#define CPU_ADDR_NMI                CPU_ADDR_RESERVED_START
#define CPU_ADDR_RESET              CPU_ADDR_RESERVED_START + 2
#define CPU_ADDR_BREAK              CPU_ADDR_RESERVED_START + 4

typedef enum {
    REG_INVALID = -1,
    REG_SP,
    REG_ACC,
    REG_X,
    REG_Y,
    REG_CPU,
    REG_MAX
} short_reg_t;

void core_clear_all();

uint8_t MEM_READ(uint16_t addr);
uint16_t MEM_READ16(uint16_t addr);
void MEM_WRITE(uint16_t addr, uint8_t v);
void MEM_WRITE16(uint16_t addr, uint16_t v);
void MEM_INC(uint16_t addr, int8_t v);

uint8_t STACK_POP();
uint16_t STACK_POP16();
void STACK_PUSH(uint8_t v);
void STACK_PUSH16(uint16_t v);

uint8_t GET_SREG(short_reg_t reg);
int GET_FLAG(uint8_t flag);
uint16_t GET_PC();

void SET_SREG(short_reg_t reg, uint8_t v);
void SET_FLAG(uint8_t flag, int v);
void SET_PC(uint16_t v);
void SET_PC_COND(uint16_t v, uint8_t condition);

#endif
