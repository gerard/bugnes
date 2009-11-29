#include <check.h>
#include "ppu_registers.h"

START_TEST (CR1SetAllFields)
{
    PPU_REG_SETF(CR1, NAME_TABLE_ADDR, 3);
    PPU_REG_SETF(CR1, VERTICAL_WRITE, 1);
    PPU_REG_SETF(CR1, SPRITE_PATT_TABLE, 1);
    PPU_REG_SETF(CR1, SCREEN_PATT_TABLE, 1);
    PPU_REG_SETF(CR1, SPRITE_SIZE, 1);
    PPU_REG_SETF(CR1, PPU_MASTER_SLAVE, 1);
    PPU_REG_SETF(CR1, VBLANK_ENABLE, 1);

    fail_unless(PPU_REG_GET(CR1) == 0xFF);
}
END_TEST

Suite *simplenes_suite(void)
{
    Suite *s = suite_create("NES");
    TCase *tc_main = tcase_create("main");
    tcase_add_test(tc_main, CR1SetAllFields);
    suite_add_tcase(s, tc_main);

    return s;
}
