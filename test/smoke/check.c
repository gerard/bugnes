#include <check.h>

START_TEST (SmokeTest)
{
    fail_unless(1 == 1);
}
END_TEST

Suite *smoke_suite(void)
{
    Suite *s = suite_create("SMOKE");
    TCase *tc_main = tcase_create("main");
    tcase_add_test(tc_main, SmokeTest);
    suite_add_tcase(s, tc_main);

    return s;
}
