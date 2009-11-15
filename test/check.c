#include <stdlib.h>
#include <stdint.h>
#include <check.h>

#include <cpu/sfot.h>
#include "6502_macros.h"

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
}

/* Same thing for reads */
static unsigned int write_mh_hit_times;
uint8_t write_mh_hit(uint16_t addr, uint8_t value)
{
    write_mh_hit_times++;
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

    read_mh_hit_times = 0;
    write_mh_hit_times = 0;
    sfot_memhook_insert_read_simple(read_mh_hit, 0x0);
    sfot_memhook_insert_write_simple(write_mh_hit, 0x0);
    sfot_memhook_insert_transl_simple(transl_mh_mirror, 0x80);
    sfot_excepthook_insert(EXCEPTHOOK_TYPE_BRK, exception_hook);
    sfot_install_step_cb(step_hook_stop_at_brk);

    sfot_load_stream(ASM, 0x200, asm_size);
    sfot_set_reset(0x200);
    sfot_poweron(SFOT_RUN_MAIN);

    fail_if(read_mh_hit_times == 0, "Read hook not hit");
    fail_if(read_mh_hit_times >  1, "Read hook hit multiple times");
    fail_if(write_mh_hit_times == 0, "Write hook not hit");
    fail_if(write_mh_hit_times >  1, "Write hook hit multiple times");
    fail_unless(exception_addr == EXCEPTION_SUCCESS,
                "Except return address signals FAIL");
}
END_TEST

Suite *sfot_suite(void)
{
    Suite *s = suite_create("SFOT");
    TCase *tc_transl_hook = tcase_create("TranslationHook");
    tcase_add_test(tc_transl_hook, MirrorWithTranslationHook);
    suite_add_tcase(s, tc_transl_hook);

    return s;
}

int main(void)
{
    int n_failed;
    Suite *s = sfot_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    n_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return n_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
