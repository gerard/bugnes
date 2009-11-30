#include <check.h>
#include <stdint.h>
#include "ppu.h"
#include "ppu_registers.h"

static uint8_t demux16_high(uint16_t v)  { return v >> 8; }
static uint8_t demux16_low(uint16_t v)   { return v & 8; }

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

START_TEST (PPUMemoryReadWriteInc1)
{
    uint8_t val1 = 0x55;
    uint8_t val2 = 0xAA;
    uint16_t addr = 0x1555;

    /* Set autoincrement to 0 => increment of 1 */
    PPU_REG_SETF(CR1, VERTICAL_WRITE, 0);
    PPU_hook_memaddr_write(0x2006, demux16_high(addr));
    PPU_hook_memaddr_write(0x2006, demux16_low(addr));
    PPU_hook_memdata_write(0x2007, val1);
    PPU_hook_memdata_write(0x2007, val2);

    PPU_hook_memaddr_write(0x2006, demux16_high(addr));
    PPU_hook_memaddr_write(0x2006, demux16_low(addr));

    /* First read is invalid */
    (void)PPU_hook_memdata_read(0x2007);

    fail_unless(PPU_hook_memdata_read(0x2007) == 0x55);
    fail_unless(PPU_hook_memdata_read(0x2007) == 0xAA);
}
END_TEST

START_TEST (PPUMemoryReadWriteInc32)
{
    uint8_t val1 = 0x55;
    uint8_t val2 = 0xAA;
    uint16_t addr = 0x1555;

    /* Set autoincrement to 0 => increment of 1 */
    PPU_REG_SETF(CR1, VERTICAL_WRITE, 1);
    PPU_hook_memaddr_write(0x2006, demux16_high(addr));
    PPU_hook_memaddr_write(0x2006, demux16_low(addr));
    PPU_hook_memdata_write(0x2007, val1);
    PPU_hook_memdata_write(0x2007, val2);

    PPU_hook_memaddr_write(0x2006, demux16_high(addr));
    PPU_hook_memaddr_write(0x2006, demux16_low(addr));

    /* First read is invalid */
    (void)PPU_hook_memdata_read(0x2007);

    fail_unless(PPU_hook_memdata_read(0x2007) == 0x55);
    fail_unless(PPU_hook_memdata_read(0x2007) == 0xAA);
}
END_TEST

Suite *simplenes_suite(void)
{
    Suite *s = suite_create("NES");
    TCase *tc_main = tcase_create("main");
    tcase_add_test(tc_main, CR1SetAllFields);
    tcase_add_test(tc_main, PPUMemoryReadWriteInc1);
    tcase_add_test(tc_main, PPUMemoryReadWriteInc32);
    suite_add_tcase(s, tc_main);

    return s;
}
