#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "opcodes-private.h"
#include "opcodes.h"
#include "excepthooks.h"
#include "core.h"
#include "addr.h"
#include "disas.h"

/*
 * BIG TODO LIST:
 *  o Decimal (honoring the D flag)
 *  o Interrupts?
 */

/* This is setter/getter.  As soon as you read the value you need to set it
 * again.  Be careful.
 */
static uint16_t branch_status(uint16_t new)
{
    static uint16_t reg_pc_branched;
    uint16_t old;

    old = reg_pc_branched;
    reg_pc_branched = new;

    return old;
}

/********
 * PC Instruction fetch/jump
 **/
static uint16_t opcode_len()
{
    switch(opcode_names[MEM_READ(GET_PC())].addr_mode) {
        case A_IMP:
        case A_ACC: return 1;
        case A_IMM:
        case A_ZPA:
        case A_ZPX:
        case A_ZPY:
        case A_REL:
        case A_INX:
        case A_INY: return 2;
        case A_ABS:
        case A_ABX:
        case A_ABY:
        case A_IND: return 3;
        default:
            assert(0);
    }
}

/********
 * Flag setting routines.
 * Note that most routines avoid to clear the flags if the condition is not
 * met.  For instance N and Z are set but cleared always manually.  Although
 * I'm not sure about this behaviour, this is how this link describes it:
 *     o- http://www.obelisk.demon.co.uk/6502/
 *
 * TODO: Investigate whether that's true.
 *
 **** Basic flags, that is, N and Z, whose semantics doesn't change ****/
void flags_update_basic_by_value(uint8_t value)
{
    if (value == 0) SET_FLAG(FLAG_Z, 1);
    else SET_FLAG(FLAG_Z, 0);

    if ((int8_t)value < 0) SET_FLAG(FLAG_N, 1);
    else SET_FLAG(FLAG_N, 0);
}

void flags_update_basic_by_register(short_reg_t reg)
{
    /* These 2 don't update any flags */
    if (reg == REG_CPU) return;
    if (reg == REG_SP) return;

    flags_update_basic_by_value(GET_SREG(reg));
}

void flags_update_basic_by_mem(uint16_t addr)
{
    flags_update_basic_by_value(MEM_READ(addr));
}

/**** Flags with different semantics for different op classes ****/
void flags_update_logic_overflow(uint8_t value)
{
    if (value & (1 << 6)) SET_FLAG(FLAG_V, 1);
}

void flags_update_arith_overflow(int8_t a, int8_t b, int8_t factor)
{
    assert(abs(factor) == 1);
    int16_t long_value = (int16_t)a + (int16_t)(b*factor);
    if (long_value != (int16_t)(a + (b*factor))) SET_FLAG(FLAG_V, 1);
    else SET_FLAG(FLAG_V, 0);
}

void flags_update_arith_carry(uint8_t a, uint8_t b, int8_t factor)
{
    assert(abs(factor) == 1);
    uint16_t long_value = (uint16_t)a + (uint16_t)(b*factor);
    if (long_value != (uint16_t)(a + (b*factor))) SET_FLAG(FLAG_C, 1);
    else SET_FLAG(FLAG_C, 0);
}

void flags_update_shrot_carry(uint8_t value, int direction, int fill)
{
    assert(abs(direction) == 1);
    assert(!fill | (fill == 1));

    SET_FLAG(FLAG_C, 0);
    if (direction > 0 && (value & (1 << 7))) SET_FLAG(FLAG_C, 1);
    if (direction < 0 && (value & (1 << 0))) SET_FLAG(FLAG_C, 1);
}

/********
 * ASM Opcode primitives
 *
 **** Transfer Ops ****/
static void asm_reg2reg(short_reg_t dest, short_reg_t source)
{
    SET_SREG(dest, GET_SREG(source));
    flags_update_basic_by_register(dest);
}

static void asm_reg2mem(uint16_t dest_addr, short_reg_t source)
{
    MEM_WRITE(dest_addr, GET_SREG(source));
    /* We have set a value in memory: no flags update */
}

static void asm_mem2reg(short_reg_t dest, uint16_t source_addr)
{
    SET_SREG(dest, MEM_READ(source_addr));
    flags_update_basic_by_register(dest);
}

static void asm_reg2stack(short_reg_t source)
{
    STACK_PUSH(GET_SREG(source));
    /* Value in memory again: no flags touched */
}

static void asm_stack2reg(short_reg_t dest)
{
    SET_SREG(dest, STACK_POP());
    flags_update_basic_by_register(dest);
}

static void asm_pc2stack()
{
    STACK_PUSH16(GET_PC() + 2);
}

/* Instead of actually setting REG_PC, we just go and set up branch_status
 * so instruction_fetch will get our message
 */
static void asm_stack2pc()
{
    branch_status(STACK_POP16() + 1);
}

static void asm_mem2pc(uint16_t addr, int condition)
{
    if (condition) branch_status(addr);
}


/**** Increments/Decrements ****/
static void asm_inc_mem(uint16_t addr, int8_t inc)
{
    assert(abs(inc) == 1);
    MEM_INC(addr, inc);
    flags_update_basic_by_mem(addr);
}

static void asm_inc_reg(short_reg_t reg, int8_t inc)
{
    assert(abs(inc) == 1);
    SET_SREG(reg, GET_SREG(reg) + inc);
    flags_update_basic_by_register(reg);
}


/**** Logic Ops ****/
/* This op takes REG_INVALID for BIT operations */
static void asm_logic_and(short_reg_t reg, uint8_t value)
{
    uint8_t result = GET_SREG(reg) & value;

    flags_update_basic_by_value(result);
    if (reg == REG_INVALID) flags_update_logic_overflow(result);

    SET_SREG(reg, result);
}

static void asm_logic_eor(short_reg_t reg, uint8_t value)
{
    uint8_t result = GET_SREG(reg) ^ value;

    flags_update_basic_by_value(GET_SREG(reg));

    SET_SREG(reg, result);
}

static void asm_logic_ora(short_reg_t reg, uint8_t value)
{
    uint8_t result = GET_SREG(reg) | value;

    flags_update_basic_by_value(GET_SREG(reg));

    SET_SREG(reg, result);
}


/**** Arithmetic Ops ****/
static void asm_arith_add(short_reg_t dest, short_reg_t source, uint8_t value)
{
    uint8_t result = GET_SREG(source) + value;

    flags_update_basic_by_value(result);
    flags_update_arith_carry(GET_SREG(source), value, 1);
    flags_update_arith_overflow(GET_SREG(source), value, 1);

    SET_SREG(dest, result);
}

/* This op takes REG_INVALID for CMP operations */
static void asm_arith_sub(short_reg_t dest, short_reg_t source, uint8_t value)
{
    uint8_t result = GET_SREG(source) - value;

    flags_update_basic_by_value(result);
    flags_update_arith_carry(GET_SREG(source), value, -1);
    if (source != REG_INVALID) flags_update_arith_overflow(GET_SREG(source), value, -1);

    SET_SREG(dest, result);
}


/**** Shifts ****/
/* This is quite ugly: we don't know if we are given a memory address or a
 * register.  So first we check if addr is valid.
 */
static void asm_shrot(uint16_t addr, short_reg_t reg, int direction, int fill)
{
    assert(abs(direction) == 1);
    assert(!fill | (fill == 1));

    uint8_t value = ADDR_VALID(addr) ? MEM_READ(addr) : GET_SREG(reg);
    flags_update_shrot_carry(value, direction, fill);

    if (direction > 0) {
        value <<= 1;
        value |= fill;
    } else {
        value >>= 1;
        value |= (fill << 7);
    }

    if (ADDR_VALID(addr)) {
        MEM_WRITE(addr, value);
        flags_update_basic_by_mem(addr);
    } else {
        SET_SREG(reg, value);
        flags_update_basic_by_register(reg);
    }
}


/**** Misc ****/
static void asm_break()
{
    excepthook_check(EXCEPTHOOK_TYPE_BRK, "BRK");
    SET_PC(MEM_READ16(CPU_ADDR_BREAK));
}

/* Move: Load/Store */
static void asm_LDA(addr_mode_t m)   { asm_mem2reg(REG_ACC, ADDR(m)); }
static void asm_LDX(addr_mode_t m)   { asm_mem2reg(REG_X,   ADDR(m)); }
static void asm_LDY(addr_mode_t m)   { asm_mem2reg(REG_Y,   ADDR(m)); }
static void asm_STA(addr_mode_t m)   { asm_reg2mem(ADDR(m), REG_ACC); }
static void asm_STX(addr_mode_t m)   { asm_reg2mem(ADDR(m), REG_X); }
static void asm_STY(addr_mode_t m)   { asm_reg2mem(ADDR(m), REG_Y); }

/* Move: Register Transfers */
static void asm_TAX(addr_mode_t m)   { asm_reg2reg(REG_X, REG_ACC); }
static void asm_TAY(addr_mode_t m)   { asm_reg2reg(REG_Y, REG_ACC); }
static void asm_TXA(addr_mode_t m)   { asm_reg2reg(REG_ACC, REG_X); }
static void asm_TYA(addr_mode_t m)   { asm_reg2reg(REG_ACC, REG_Y); }

/* Move: Stack Operations */
static void asm_TSX(addr_mode_t m)   { asm_reg2reg(REG_X, REG_SP); }
static void asm_TXS(addr_mode_t m)   { asm_reg2reg(REG_SP, REG_X); }
static void asm_PHA(addr_mode_t m)   { asm_reg2stack(REG_ACC); }
static void asm_PHP(addr_mode_t m)   { asm_reg2stack(REG_CPU); }
static void asm_PLA(addr_mode_t m)   { asm_stack2reg(REG_ACC); }
static void asm_PLP(addr_mode_t m)   { asm_stack2reg(REG_CPU); }

/* Logical */
/* Logical opcodes operate with the accumulator, but the addressing mode can
 * address memory or immediates, so we have to use ADDR_MODE_GET_VALUE()
 */
static void asm_AND(addr_mode_t m)   { asm_logic_and(REG_ACC, ADDR_MODE_GET_VALUE(m)); }
static void asm_EOR(addr_mode_t m)   { asm_logic_eor(REG_ACC, ADDR_MODE_GET_VALUE(m)); }
static void asm_ORA(addr_mode_t m)   { asm_logic_ora(REG_ACC, ADDR_MODE_GET_VALUE(m)); }
static void asm_BIT(addr_mode_t m)   { asm_logic_and(REG_INVALID, ADDR_MODE_GET_VALUE(m)); }

/* Arithmetic */
static void asm_ADC(addr_mode_t m)   { asm_arith_add(REG_ACC, REG_ACC, ADDR_MODE_GET_VALUE(m)); }
static void asm_SBC(addr_mode_t m)   { asm_arith_sub(REG_ACC, REG_ACC, ADDR_MODE_GET_VALUE(m)); }
static void asm_CMP(addr_mode_t m)   { asm_arith_sub(REG_INVALID, REG_ACC, ADDR_MODE_GET_VALUE(m)); }
static void asm_CPX(addr_mode_t m)   { asm_arith_sub(REG_INVALID, REG_X, ADDR_MODE_GET_VALUE(m)); }
static void asm_CPY(addr_mode_t m)   { asm_arith_sub(REG_INVALID, REG_Y, ADDR_MODE_GET_VALUE(m)); }

/* Increments & Decrements */
static void asm_INC(addr_mode_t m)   { asm_inc_mem(ADDR(m),  1); }
static void asm_INX(addr_mode_t m)   { asm_inc_reg(REG_X,    1); }
static void asm_INY(addr_mode_t m)   { asm_inc_reg(REG_Y,    1); }
static void asm_DEC(addr_mode_t m)   { asm_inc_mem(ADDR(m), -1); }
static void asm_DEX(addr_mode_t m)   { asm_inc_reg(REG_X,   -1); }
static void asm_DEY(addr_mode_t m)   { asm_inc_reg(REG_Y,   -1); }

/* Shifts */
static void asm_ASL(addr_mode_t m)   { asm_shrot(ADDR(m), REG_ACC,  1, 0); }
static void asm_LSR(addr_mode_t m)   { asm_shrot(ADDR(m), REG_ACC, -1, 0); }
static void asm_ROL(addr_mode_t m)   { asm_shrot(ADDR(m), REG_ACC,  1, GET_FLAG(FLAG_C)); }
static void asm_ROR(addr_mode_t m)   { asm_shrot(ADDR(m), REG_ACC, -1, GET_FLAG(FLAG_C)); }

/* Jumps & Calls */
/* JSR pushes the address of the return address minus one */
static void asm_JMP(addr_mode_t m)   { asm_mem2pc(ADDR(m), 1); }
static void asm_JSR(addr_mode_t m)   { asm_pc2stack(); asm_mem2pc(ADDR(m), 1); }
static void asm_RTS(addr_mode_t m)   { asm_stack2pc(); }

/* Branches */
static void asm_BCC(addr_mode_t m)   { asm_mem2pc(ADDR(m), !GET_FLAG(FLAG_C)); }
static void asm_BCS(addr_mode_t m)   { asm_mem2pc(ADDR(m),  GET_FLAG(FLAG_C)); }
static void asm_BEQ(addr_mode_t m)   { asm_mem2pc(ADDR(m),  GET_FLAG(FLAG_Z)); }
static void asm_BMI(addr_mode_t m)   { asm_mem2pc(ADDR(m),  GET_FLAG(FLAG_N)); }
static void asm_BNE(addr_mode_t m)   { asm_mem2pc(ADDR(m), !GET_FLAG(FLAG_Z)); }
static void asm_BPL(addr_mode_t m)   { asm_mem2pc(ADDR(m), !GET_FLAG(FLAG_N)); }
static void asm_BVC(addr_mode_t m)   { asm_mem2pc(ADDR(m), !GET_FLAG(FLAG_V)); }
static void asm_BVS(addr_mode_t m)   { asm_mem2pc(ADDR(m),  GET_FLAG(FLAG_V)); }

/* Status Flag Changes */
static void asm_CLC(addr_mode_t m)   { SET_FLAG(FLAG_C, 0); }
static void asm_CLD(addr_mode_t m)   { SET_FLAG(FLAG_D, 0); }
static void asm_CLI(addr_mode_t m)   { SET_FLAG(FLAG_I, 0); }
static void asm_CLV(addr_mode_t m)   { SET_FLAG(FLAG_V, 0); }
static void asm_SEC(addr_mode_t m)   { SET_FLAG(FLAG_C, 1); }
static void asm_SED(addr_mode_t m)   { SET_FLAG(FLAG_D, 1); }
static void asm_SEI(addr_mode_t m)   { SET_FLAG(FLAG_I, 1); }

/* System Functions */
static void asm_BRK(addr_mode_t m)   { asm_break(); }
static void asm_NOP(addr_mode_t m)   {}
static void asm_RTI(addr_mode_t m)   {}


/**
 * PUBLIC: Opcode implementation.  The logic on the flag setting is offloaded
 * to the flags_update_*() functions.
 */

void opcode_fetch_start(uint16_t addr)
{
    branch_status(addr);
}

void opcode_fetch()
{
    uint16_t branch = branch_status(INVALID_ADDRESS);
    uint16_t next = ADDR_VALID(branch) ? branch : GET_PC() + opcode_len();

    /* We shouldn't assert this, since it's up to the ROM to behave,
     * nevertheless, at this stage, I usually find that the problem is in my
     * code rather than in a malicious ROM ;).
     * PS: After giving it some thought, it *might* have been intentionally
     * used to get a couple of bytes more out of the tiny ROM space that these
     * cartrides used.  After all, as long as you get a valid address out of
     * the instruction encoding, you are fine.  Still, I will leave it for a
     * while like this.
     * PSS: Seriously, who needs a blog? :)
     */
    assert(next < CPU_ADDR_RESERVED_START);
    SET_PC(next);
}

void opcode_exec()
{
    struct sfot_opcode op = opcode_names[MEM_READ(GET_PC())];
    op.exec(op.addr_mode);
}

const char *opcode_get_name()
{
    return opcode_names[MEM_READ(GET_PC())].name;
}

int opcode_get_addr_mode()
{
    return opcode_names[MEM_READ(GET_PC())].addr_mode;
}

int opcode_get_params(uint8_t *byte1, uint8_t *byte2)
{
    *byte1 = opcode_len() > 1 ? MEM_READ(GET_PC() + 1) : 0;
    *byte2 = opcode_len() > 2 ? MEM_READ(GET_PC() + 2) : 0;

    return opcode_len() - 1;
}
