/**
 * This file contains the opcode table for the 6502 processor.
 */

#ifndef _OPCODES_PRIVATE_H_
#define _OPCODES_PRIVATE_H_
#include <stdint.h>

#include "addr.h"

/* Convenience declaration macros.  To be undefined as soon as possible. */
#define DEFINE_FUN(opc)             static void asm_##opc(addr_mode_t)
#define O(name, addr, cycles)       { #name, asm_##name, addr, cycles }
#define O_UNKNOWN                   { "XXX", asm_NOP, A_IMP, 2 }

typedef void (*fun_t)(addr_mode_t);
struct sfot_opcode {
    char name[4];
    fun_t exec;
    addr_mode_t addr_mode;
    uint8_t sets_flags;
    uint8_t base_cycles;
};

/* Move: Load/Store */
DEFINE_FUN(LDA);
DEFINE_FUN(LDX);
DEFINE_FUN(LDY);
DEFINE_FUN(STA);
DEFINE_FUN(STX);
DEFINE_FUN(STY);

/* Move: Register Transfers */
DEFINE_FUN(TAX);
DEFINE_FUN(TAY);
DEFINE_FUN(TXA);
DEFINE_FUN(TYA);

/* Move: Stack Operations */
DEFINE_FUN(TSX);
DEFINE_FUN(TXS);
DEFINE_FUN(PHA);
DEFINE_FUN(PHP);
DEFINE_FUN(PLA);
DEFINE_FUN(PLP);

/* Logical */
DEFINE_FUN(AND);
DEFINE_FUN(EOR);
DEFINE_FUN(ORA);
DEFINE_FUN(BIT);

/* Arithmetic */
DEFINE_FUN(ADC);
DEFINE_FUN(SBC);
DEFINE_FUN(CMP);
DEFINE_FUN(CPX);
DEFINE_FUN(CPY);

/* Increments & Decrements */
DEFINE_FUN(INC);
DEFINE_FUN(INX);
DEFINE_FUN(INY);
DEFINE_FUN(DEC);
DEFINE_FUN(DEX);
DEFINE_FUN(DEY);

/* Shifts */
DEFINE_FUN(ASL);
DEFINE_FUN(LSR);
DEFINE_FUN(ROL);
DEFINE_FUN(ROR);

/* Jumps & Calls */
DEFINE_FUN(JMP);
DEFINE_FUN(JSR);
DEFINE_FUN(RTS);

/* Branches */
DEFINE_FUN(BCC);
DEFINE_FUN(BCS);
DEFINE_FUN(BEQ);
DEFINE_FUN(BMI);
DEFINE_FUN(BNE);
DEFINE_FUN(BPL);
DEFINE_FUN(BVC);
DEFINE_FUN(BVS);

/* Status Flag Changes */
DEFINE_FUN(CLC);
DEFINE_FUN(CLD);
DEFINE_FUN(CLI);
DEFINE_FUN(CLV);
DEFINE_FUN(SEC);
DEFINE_FUN(SED);
DEFINE_FUN(SEI);

/* System Functions */
DEFINE_FUN(BRK);
DEFINE_FUN(NOP);
DEFINE_FUN(RTI);


/* If the opcode is not known, we default to a NOP */
struct sfot_opcode opcode_names[] = { 
/* 0x00 */ O(BRK, A_IMP, 7), O(ORA, A_INX, 6),        O_UNKNOWN,        O_UNKNOWN,
/* 0x04 */        O_UNKNOWN, O(ORA, A_ZPA, 3), O(ASL, A_ZPA, 5),        O_UNKNOWN,
/* 0x08 */ O(PHP, A_IMP, 3), O(ORA, A_IMM, 2), O(ASL, A_ACC, 2),        O_UNKNOWN,
/* 0x0C */        O_UNKNOWN, O(ORA, A_ABS, 4), O(ASL, A_ABS, 6),        O_UNKNOWN,
/* 0x10 */ O(BPL, A_REL, 2), O(ORA, A_INY, 5),        O_UNKNOWN,        O_UNKNOWN,
/* 0x14 */        O_UNKNOWN, O(ORA, A_ZPX, 4), O(ASL, A_ZPX, 6),        O_UNKNOWN,
/* 0x18 */ O(CLC, A_IMP, 2), O(ORA, A_ABY, 4),        O_UNKNOWN,        O_UNKNOWN,
/* 0x1C */        O_UNKNOWN, O(ORA, A_ABX, 4), O(ASL, A_ABX, 7),        O_UNKNOWN,
/* 0x20 */ O(JSR, A_ABS, 6), O(AND, A_INX, 6),        O_UNKNOWN,        O_UNKNOWN,
/* 0x24 */ O(BIT, A_ZPA, 3), O(AND, A_ZPA, 3), O(ROL, A_ZPA, 5),        O_UNKNOWN,
/* 0x28 */ O(PLP, A_IMP, 4), O(AND, A_IMM, 2), O(ROL, A_ACC, 2),        O_UNKNOWN,
/* 0x2C */ O(BIT, A_ABS, 4), O(AND, A_ABS, 4), O(ROL, A_ABS, 6),        O_UNKNOWN,
/* 0x30 */ O(BMI, A_REL, 2), O(AND, A_INY, 5),        O_UNKNOWN,        O_UNKNOWN,
/* 0x34 */        O_UNKNOWN, O(AND, A_ZPX, 4), O(ROL, A_ZPX, 6),        O_UNKNOWN,
/* 0x38 */ O(SEC, A_IMP, 2), O(AND, A_ABY, 4),        O_UNKNOWN,        O_UNKNOWN,
/* 0x3C */        O_UNKNOWN, O(AND, A_ABX, 4), O(ROL, A_ABX, 7),        O_UNKNOWN,
/* 0x40 */ O(RTI, A_IMP, 6), O(EOR, A_INX, 6),        O_UNKNOWN,        O_UNKNOWN,
/* 0x44 */        O_UNKNOWN, O(EOR, A_ZPA, 3), O(LSR, A_ZPA, 5),        O_UNKNOWN,
/* 0x48 */ O(PHA, A_IMP, 3), O(EOR, A_IMM, 2), O(LSR, A_ACC, 2),        O_UNKNOWN,
/* 0x4C */ O(JMP, A_ABS, 3), O(EOR, A_ABS, 4), O(LSR, A_ABS, 6),        O_UNKNOWN,
/* 0x50 */ O(BVC, A_REL, 2), O(EOR, A_INY, 5),        O_UNKNOWN,        O_UNKNOWN,
/* 0x54 */        O_UNKNOWN, O(EOR, A_ZPX, 4), O(LSR, A_ZPX, 6),        O_UNKNOWN,
/* 0x58 */ O(CLI, A_IMP, 2), O(EOR, A_ABY, 4),        O_UNKNOWN,        O_UNKNOWN,
/* 0x5C */        O_UNKNOWN, O(EOR, A_ABX, 4), O(LSR, A_ABX, 7),        O_UNKNOWN,
/* 0x60 */ O(RTS, A_IMP, 6), O(ADC, A_INX, 6),        O_UNKNOWN,        O_UNKNOWN,
/* 0x64 */        O_UNKNOWN, O(ADC, A_ZPA, 3), O(ROR, A_ZPA, 5),        O_UNKNOWN,
/* 0x68 */ O(PLA, A_IMP, 4), O(ADC, A_IMM, 2), O(ROR, A_ACC, 2),        O_UNKNOWN,
/* 0x6C */ O(JMP, A_IND, 5), O(ADC, A_ABS, 4), O(ROR, A_ABS, 6),        O_UNKNOWN,
/* 0x70 */ O(BVS, A_REL, 2), O(ADC, A_INY, 5),        O_UNKNOWN,        O_UNKNOWN,
/* 0x74 */        O_UNKNOWN, O(ADC, A_ZPX, 4), O(ROR, A_ZPX, 6),        O_UNKNOWN,
/* 0x78 */ O(SEI, A_IMP, 2), O(ADC, A_ABY, 4),        O_UNKNOWN,        O_UNKNOWN,
/* 0x7C */        O_UNKNOWN, O(ADC, A_ABX, 4), O(ROR, A_ABX, 7),        O_UNKNOWN,
/* 0x80 */        O_UNKNOWN, O(STA, A_INX, 6),        O_UNKNOWN,        O_UNKNOWN,
/* 0x84 */ O(STY, A_ZPA, 3), O(STA, A_ZPA, 3), O(STX, A_ZPA, 3),        O_UNKNOWN,
/* 0x88 */ O(DEY, A_IMP, 2),        O_UNKNOWN, O(TXA, A_IMP, 2),        O_UNKNOWN,
/* 0x8C */ O(STY, A_ABX, 4), O(STA, A_ABS, 4), O(STX, A_ABS, 4),        O_UNKNOWN,
/* 0x90 */ O(BCC, A_REL, 2), O(STA, A_INY, 6),        O_UNKNOWN,        O_UNKNOWN,
/* 0x94 */ O(STY, A_ZPY, 4), O(STA, A_ZPX, 4), O(STX, A_ZPY, 4),        O_UNKNOWN,
/* 0x98 */ O(TYA, A_IMP, 2), O(STA, A_ABY, 5), O(TXS, A_IMP, 2),        O_UNKNOWN,
/* 0x9C */        O_UNKNOWN, O(STA, A_ABX, 5),        O_UNKNOWN,        O_UNKNOWN,
/* 0xA0 */ O(LDY, A_IMM, 2), O(LDA, A_INX, 6), O(LDX, A_IMM, 2),        O_UNKNOWN,
/* 0xA4 */ O(LDY, A_ZPA, 3), O(LDA, A_ZPA, 3), O(LDX, A_ZPA, 3),        O_UNKNOWN,
/* 0xA8 */ O(TAY, A_IMP, 2), O(LDA, A_IMM, 2), O(TAX, A_IMP, 2),        O_UNKNOWN,
/* 0xAC */ O(LDY, A_ABS, 4), O(LDA, A_ABS, 4), O(LDX, A_ABS, 4),        O_UNKNOWN,
/* 0xB0 */ O(BCS, A_REL, 2), O(LDA, A_INY, 5),        O_UNKNOWN,        O_UNKNOWN,
/* 0xB4 */ O(LDY, A_ZPX, 4), O(LDA, A_ZPX, 4), O(LDX, A_ZPY, 4),        O_UNKNOWN,
/* 0xB8 */ O(CLV, A_IMP, 2), O(LDA, A_ABY, 4), O(TSX, A_IMP, 2),        O_UNKNOWN,
/* 0xBC */ O(LDY, A_ABX, 3), O(LDA, A_ABX, 4), O(LDX, A_ABY, 4),        O_UNKNOWN,
/* 0xC0 */ O(CPY, A_IMM, 2), O(CMP, A_INX, 6),        O_UNKNOWN,        O_UNKNOWN,
/* 0xC4 */ O(CPY, A_ZPA, 3), O(CMP, A_ZPA, 3), O(DEC, A_ZPA, 5),        O_UNKNOWN,
/* 0xC8 */ O(INY, A_IMP, 2), O(CMP, A_IMM, 2), O(DEX, A_IMP, 2),        O_UNKNOWN,
/* 0xCC */ O(CPY, A_ABS, 4), O(CMP, A_ABS, 4), O(DEC, A_ABS, 6),        O_UNKNOWN,
/* 0xD0 */ O(BNE, A_REL, 2), O(CMP, A_INY, 5),        O_UNKNOWN,        O_UNKNOWN,
/* 0xD4 */        O_UNKNOWN, O(CMP, A_ZPX, 4), O(DEC, A_ZPX, 6),        O_UNKNOWN,
/* 0xD8 */ O(CLD, A_IMP, 2), O(CMP, A_ABY, 4),        O_UNKNOWN,        O_UNKNOWN,
/* 0xDC */        O_UNKNOWN, O(CMP, A_ABX, 4), O(DEC, A_ABX, 7),        O_UNKNOWN,
/* 0xE0 */ O(CPX, A_IMM, 2), O(SBC, A_INX, 6),        O_UNKNOWN,        O_UNKNOWN,
/* 0xE4 */ O(CPX, A_ZPA, 3), O(SBC, A_ZPA, 3), O(INC, A_ZPA, 5),        O_UNKNOWN,
/* 0xE8 */ O(INX, A_IMP, 2), O(SBC, A_IMM, 2), O(NOP, A_IMP, 2),        O_UNKNOWN,
/* 0xEC */ O(CPX, A_ABS, 4), O(SBC, A_ABS, 4), O(INC, A_ABS, 6),        O_UNKNOWN,
/* 0xF0 */ O(BEQ, A_REL, 2), O(SBC, A_INY, 5),        O_UNKNOWN,        O_UNKNOWN,
/* 0xF4 */        O_UNKNOWN, O(SBC, A_ZPX, 4), O(INC, A_ZPX, 6),        O_UNKNOWN,
/* 0xF8 */ O(SED, A_IMP, 2), O(SBC, A_ABY, 4),        O_UNKNOWN,        O_UNKNOWN,
/* 0xFC */        O_UNKNOWN, O(SBC, A_ABX, 4), O(INC, A_ABX, 7),        O_UNKNOWN
};

/* TODO: Work out the undocumented opcodes */
#undef DEFINE_FUN
#undef O_UNKNOWN
#undef O

#endif
