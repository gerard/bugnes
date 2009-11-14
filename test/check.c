#include <stdlib.h>
#include <stdint.h>
#include <check.h>

#include <cpu/sfot.h>

static const uint8_t ASM_store_load_at_ZP[] = {
    0xA9, 0x01, /* LDA #$01 */
    0x85, 0x80, /* STA $80  */
    0xA5, 0x80, /* LDA $80  */
};

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

int step_hook_stop_at_brk(struct sfot_step_info *info)
{
    return !strncmp(info->opcode_name, "BRK", 3);
}


START_TEST (MirrorWithTranslationHook)
{
    read_mh_hit_times = 0;
    write_mh_hit_times = 0;
    sfot_memhook_insert_read_simple(read_mh_hit, 0x0);
    sfot_memhook_insert_write_simple(write_mh_hit, 0x0);
    sfot_memhook_insert_transl_simple(transl_mh_mirror, 0x80);
    sfot_install_step_cb(step_hook_stop_at_brk);

    sfot_load_stream(ASM_store_load_at_ZP, 0x100, sizeof(ASM_store_load_at_ZP));
    sfot_set_reset(0x100);
    sfot_poweron(SFOT_RUN_MAIN);

    fail_if(read_mh_hit_times == 0, "Read hook not hit");
    fail_if(read_mh_hit_times >  1, "Read hook hit multiple times");
    fail_if(write_mh_hit_times == 0, "Write hook not hit");
    fail_if(write_mh_hit_times >  1, "Write hook hit multiple times");
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
