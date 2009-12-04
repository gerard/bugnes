#include <check.h>
#include <stdint.h>
#include "ppu.h"
#include "ppu_registers.h"

static uint8_t demux16_high(uint16_t v)  { return (v & 0xFF00) >> 8; }
static uint8_t demux16_low(uint16_t v)   { return v & 0xFF; }

START_TEST (CR1AllSetClearFields)
{
    fail_unless(PPU_REG_GET(CR1) == 0x00);

    PPU_REG_SETF(CR1, NAME_TABLE_ADDR, 3);
    PPU_REG_SETF(CR1, VERTICAL_WRITE, 1);
    PPU_REG_SETF(CR1, SPRITE_PATT_TABLE, 1);
    fail_unless(PPU_REG_GET(CR1) == 0x0F);
    PPU_REG_SETF(CR1, SCREEN_PATT_TABLE, 1);
    PPU_REG_SETF(CR1, SPRITE_SIZE, 1);
    PPU_REG_SETF(CR1, PPU_MASTER_SLAVE, 1);
    PPU_REG_SETF(CR1, VBLANK_ENABLE, 1);

    fail_unless(PPU_REG_GET(CR1) == 0xFF);

    PPU_REG_SETF(CR1, NAME_TABLE_ADDR, 0);
    PPU_REG_SETF(CR1, VERTICAL_WRITE, 0);
    PPU_REG_SETF(CR1, SPRITE_PATT_TABLE, 0);
    fail_unless(PPU_REG_GET(CR1) == 0xF0);
    PPU_REG_SETF(CR1, SCREEN_PATT_TABLE, 0);
    PPU_REG_SETF(CR1, SPRITE_SIZE, 0);
    PPU_REG_SETF(CR1, PPU_MASTER_SLAVE, 0);
    PPU_REG_SETF(CR1, VBLANK_ENABLE, 0);

    fail_unless(PPU_REG_GET(CR1) == 0x00);
}
END_TEST

START_TEST (CR2PatternSetFields)
{
    fail_unless(PPU_REG_GET(CR2) == 0x00);

    PPU_REG_SETF(CR2, BYTE0_UNKNOWN, 1);
    PPU_REG_SETF(CR2, IMAGE_MASK, 0);
    PPU_REG_SETF(CR2, SPRITE_MASK, 1);
    PPU_REG_SETF(CR2, SCREEN_ENABLE, 0);
    PPU_REG_SETF(CR2, SPRITES_ENABLE, 1);
    PPU_REG_SETF(CR2, BACKGROUND_COLOR, 2);

    fail_unless(PPU_REG_GET(CR2) == 0x55);

    PPU_REG_SETF(CR2, BYTE0_UNKNOWN, 0);
    PPU_REG_SETF(CR2, IMAGE_MASK, 1);
    PPU_REG_SETF(CR2, SPRITE_MASK, 0);
    PPU_REG_SETF(CR2, SCREEN_ENABLE, 1);
    PPU_REG_SETF(CR2, SPRITES_ENABLE, 0);
    PPU_REG_SETF(CR2, BACKGROUND_COLOR, 5);

    fail_unless(PPU_REG_GET(CR2) == 0xAA);
}
END_TEST

START_TEST (SRSetClearByField)
{
    PPU_REG_SETF(SR, BYTES05_UNKNOWN, 0x15);
    fail_unless(PPU_REG_GETF(SR, BYTES05_UNKNOWN) == 0x15);

    PPU_REG_SETF(SR, HIT_FLAG, 1);
    fail_unless(PPU_REG_GETF(SR, HIT_FLAG) == 1);

    PPU_REG_SETF(SR, BYTES05_UNKNOWN, 0x2A);
    fail_unless(PPU_REG_GETF(SR, BYTES05_UNKNOWN) == 0x2A);
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

    fail_unless(PPU_hook_memdata_read(0x2007) == val1);
    fail_unless(PPU_hook_memdata_read(0x2007) == val2);
}
END_TEST

START_TEST (PPUMemoryReadWriteInc32)
{
    uint8_t val1 = 0x55;
    uint8_t val2 = 0xAA;
    uint16_t addr = 0x1555;

    /* Set autoincrement to 1 => increment of 32 */
    PPU_REG_SETF(CR1, VERTICAL_WRITE, 1);
    PPU_hook_memaddr_write(0x2006, demux16_high(addr));
    PPU_hook_memaddr_write(0x2006, demux16_low(addr));
    PPU_hook_memdata_write(0x2007, val1);
    PPU_hook_memdata_write(0x2007, val2);

    /* Jump straight to the increment */
    PPU_hook_memaddr_write(0x2006, demux16_high(addr + 32));
    PPU_hook_memaddr_write(0x2006, demux16_low(addr + 32));

    /* First read is invalid */
    (void)PPU_hook_memdata_read(0x2007);

    fail_unless(PPU_hook_memdata_read(0x2007) == val2);
}
END_TEST

Suite *simplenes_suite(void)
{
    Suite *s = suite_create("NES");
    TCase *tc_main = tcase_create("main");
    tcase_add_test(tc_main, CR1AllSetClearFields);
    tcase_add_test(tc_main, CR2PatternSetFields);
    tcase_add_test(tc_main, SRSetClearByField);
    tcase_add_test(tc_main, PPUMemoryReadWriteInc1);
    tcase_add_test(tc_main, PPUMemoryReadWriteInc32);
    suite_add_tcase(s, tc_main);

    return s;
}
