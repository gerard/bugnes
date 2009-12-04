#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <check.h>

#include <cpu/sfot.h>
#include "6502_macros.h"

#define JMP_TO_SUCC()   JMP_ABS(0x1000)
#define JMP_TO_FAIL()   JMP_ABS(0x1001)

#define CHECK_EXC_RET() fail_unless(exception_addr == EXCEPTION_SUCCESS, \
                                    "Except return address signals FAIL (0x%04X)", \
                                    exception_addr)

/*
 * Testing code.  The tests use hooks to gain information of the execution as
 * well as list of exception addresses.  That is, the 6502 code will jump to
 * one of this positions depending on the success or not of the execution:
 */

enum {
    EXCEPTION_SUCCESS   = 0x1000,
    EXCEPTION_FAILURE,
    EXCEPTION_MAX,
};

/*
 * List of useful hooks used through the tests
 */

/* Simple memhook that increases a counter every time it is hit */
static unsigned int read_mh_hit_times;
uint8_t read_mh_hit(uint16_t addr)
{
    read_mh_hit_times++;
    return 0;
}

/* Same thing for reads */
static unsigned int write_mh_hit_times;
uint8_t write_mh_hit(uint16_t addr, uint8_t value)
{
    write_mh_hit_times++;
    return 0;
}

/* This mimics DMA access to sprite memory in the NES */
uint8_t dma_memory_read[0x100];
uint8_t write_mh_trigger_dma(uint16_t addr, uint8_t value)
{
    sfot_do_dma(value * 0x100, dma_memory_read, 0x100);
    return value;
}

/* Mirrors 0x80 in 0x0 */
uint16_t transl_mh_mirror(uint16_t addr) {
    if (addr == 0x80) return 0;
    else return addr;
}

/* Stops execution when BRK is reached */
int step_hook_stop_at_brk(struct sfot_step_info *info)
{
    return !strncmp(info->opcode_name, "BRK", 3);
}

/* Stores exception address */
uint16_t exception_addr;
void exception_hook(uint16_t addr, const char *s)
{
    exception_addr = addr;
}

/* Test Fixtures */
void fixture_setup()
{
    read_mh_hit_times = 0;
    write_mh_hit_times = 0;

    sfot_excepthook_insert(EXCEPTHOOK_TYPE_BRK, exception_hook);
    sfot_install_step_cb(step_hook_stop_at_brk);
    sfot_set_reset(0x200);
}

void fixture_teardown()
{
    read_mh_hit_times = 0;
    write_mh_hit_times = 0;

    sfot_poweroff();
}

/*
 * Start of the actual test code
 */
START_TEST (MirrorWithTranslationHook)
{
    const uint8_t ASM[] = {
        LDA_IMM(0x01),
        STA_ZPA(0x80),
        LDA_ZPA(0x80),
        JMP_ABS(0x1000),
    };
    uint16_t asm_size = sizeof(ASM);
    sfot_load_stream(ASM, 0x200, asm_size);

    sfot_memhook_insert_read_simple(read_mh_hit, 0x0);
    sfot_memhook_insert_write_simple(write_mh_hit, 0x0);
    sfot_memhook_insert_transl_simple(transl_mh_mirror, 0x80);

    sfot_poweron(SFOT_RUN_MAIN);

    fail_if(read_mh_hit_times == 0, "Read hook not hit");
    fail_if(read_mh_hit_times >  1, "Read hook hit multiple times");
    fail_if(write_mh_hit_times == 0, "Write hook not hit");
    fail_if(write_mh_hit_times >  1, "Write hook hit multiple times");
    CHECK_EXC_RET();
}
END_TEST

START_TEST (CarryOnChainedSBC)
{
    const uint8_t ASM[] = {
        /* Set the values */
        LDA_IMM(0x00),
        STA_ZPA(0xC0),
        LDA_IMM(0x03),
        STA_ZPA(0xC1),
        LDA_IMM(0xFF),
        STA_ZPA(0xB0),
        LDA_IMM(0x00),
        STA_ZPA(0xB1),
        CLD_IMP(),
        SEC_IMP(),

        /* Do the substraction, store the result in 0xC2-0xC3 */
        LDA_ZPA(0xC0),
        SBC_ZPA(0xB0),
        STA_ZPA(0xC2),
        LDA_ZPA(0xC1),
        SBC_ZPA(0xB1),
        STA_ZPA(0xC3),

        /* Now, compare the value to the expected one: 0x201 */
        LDA_IMM(0x01),
        CMP_ZPA(0xC2),
        BEQ_REL(+3),
        JMP_TO_FAIL(),

        LDA_IMM(0x02),
        CMP_ZPA(0xC3),
        BEQ_REL(+3),
        JMP_TO_FAIL(),

        JMP_TO_SUCC(),
    };
    uint16_t asm_size = sizeof(ASM);
    sfot_load_stream(ASM, 0x200, asm_size);

    sfot_poweron(SFOT_RUN_MAIN);
    CHECK_EXC_RET();
}
END_TEST

START_TEST (SubroutineCallAndReturn)
{
    /* We jump to a subroutine that modifies the return address in the stack.
     * If we don't jump or for some reason the address is not properly altered
     * we wil fail singal fail.  The address that JSR pushes to the stack is
     * the return point *minus* one.  Anyway, that shouldn't matter.
     */
    const uint8_t ASM[] = {
        LDX_IMM(0xFF),
        TXS_IMP(),
        JSR_ABS(0x020C),
        JMP_TO_FAIL(),
        JMP_TO_SUCC(),
        PLA_IMP(),
        ADC_IMM(0x03),          /* We add 3 to avoid JMP_TO_FAIL() */
        PHA_IMP(),
        RTS_IMP(),
    };
    uint16_t asm_size = sizeof(ASM);
    sfot_load_stream(ASM, 0x200, asm_size);

    sfot_poweron(SFOT_RUN_MAIN);
    CHECK_EXC_RET();
}
END_TEST

START_TEST (DMA)
{
    const uint8_t ASM[] = {
        LDA_IMM(0xFF),
        STA_ZPA(0x33),
        STA_ZPA(0x77),
        STA_ZPA(0xBB),
        LDA_IMM(0x0),
        STA_ABS(0x400),
        JMP_TO_SUCC(),
    };
    uint16_t asm_size = sizeof(ASM);
    sfot_load_stream(ASM, 0x200, asm_size);
    sfot_memhook_insert_write_simple(write_mh_trigger_dma, 0x400);

    sfot_poweron(SFOT_RUN_MAIN);

    int i;
    for (i = 0; i < 0x100; i++) {
        switch (i) {
        case 0x33:
        case 0x77:
        case 0xBB:
            fail_unless(dma_memory_read[i] == 0xFF, "DMA Readback failed on 0x%X", i);
            break;
        default:
            fail_unless(dma_memory_read[i] == 0x0);
        }
    }

    CHECK_EXC_RET();
}
END_TEST

Suite *cpu_6502_suite(void)
{
    Suite *s = suite_create("CPU 6502");
    TCase *tc_main = tcase_create("main");
    tcase_add_checked_fixture(tc_main, fixture_setup, fixture_teardown);
    tcase_add_test(tc_main, MirrorWithTranslationHook);
    tcase_add_test(tc_main, CarryOnChainedSBC);
    tcase_add_test(tc_main, SubroutineCallAndReturn);
    tcase_add_test(tc_main, DMA);
    suite_add_tcase(s, tc_main);

    return s;
}
