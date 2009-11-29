#include <stdlib.h>
#include <check.h>

#include "cpu/6502/check.h"
#include "smoke/check.h"

int main(void)
{
    int n_failed;
    Suite *cpu_6502 = cpu_6502_suite();
    Suite *smoke = smoke_suite();

    SRunner *sr = srunner_create(cpu_6502);
    srunner_add_suite(sr, smoke);

    srunner_run_all(sr, CK_MINIMAL);

    n_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return n_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
