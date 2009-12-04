/*
 * PPU Module:  This implements the memhooks that deal with the Picture
 * Processing Unit (PPU).  The only function *not* used as such is PPU_load()
 * in charge to load the VROM from the .nes file.
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#include "ppu_registers.h"

/* MAIN PPU MEMORY */
/* memhook_PPU_memread() needs this because the first read is dummy */
static int memaddr_just_setted;
static uint16_t memaddr;

#define PPU_SIZE        0x4000
static uint8_t memory[PPU_SIZE];

static uint8_t memread() {
    uint16_t increment = PPU_REG_GETF(CR1, VERTICAL_WRITE) ? 32 : 1;
    uint8_t ret = memory[memaddr % PPU_SIZE];

    memaddr += increment;
    if (memaddr > PPU_SIZE) memaddr %= PPU_SIZE;

    return ret;
}

static uint8_t memwrite(uint8_t value)
{
    uint16_t increment = PPU_REG_GETF(CR1, VERTICAL_WRITE) ? 32 : 1;

    memory[memaddr & 0x3FFF] = value;
    memaddr += increment;
    if (memaddr > PPU_SIZE) memaddr %= PPU_SIZE;

    return value;
}


uint8_t PPU_hook_cr1_read(uint16_t addr)
{
    return PPU_REG_GET(CR1);
}

uint8_t PPU_hook_cr1_write(uint16_t addr, uint8_t value)
{
    PPU_REG_SET(CR1, value);
    return value;
}

uint8_t PPU_hook_cr2_read(uint16_t addr)
{
    return PPU_REG_GET(CR2);
}

uint8_t PPU_hook_cr2_write(uint16_t addr, uint8_t value)
{
    PPU_REG_SET(CR2, value);
    return value;
}

uint8_t PPU_hook_status_read(uint16_t addr)
{
    /* Suppose for now that VBlank and Hit flag are up always */
    return 0xC0;
}

uint8_t PPU_hook_memaddr_write(uint16_t addr, uint8_t value)
{
    static int set_lower;
    memaddr_just_setted = 1;

    if (set_lower) {
        memaddr &= 0xFF00;
        memaddr |= (uint16_t)value;
        set_lower = 0;
    } else {
        /* Only 6 lower bits are relevant */
        memaddr &= 0x00FF;
        memaddr |= (((uint16_t)value & 0x3F) << 8);
        set_lower = 1;
    }

    return value;
}

uint8_t PPU_hook_memdata_read(uint16_t addr)
{
    if (memaddr_just_setted) {
        memaddr_just_setted = 0;
        return 0;
    }
    return memread();
}

uint8_t PPU_hook_memdata_write(uint16_t addr, uint8_t value)
{
    memwrite(value);
    return 0;
}

/* XXX: We assume that fd is already lseeked to the beginning of the VROM */
int PPU_load(int fd)
{
    int bytes_read;

    if (fd < 0) return -1;

    do {
        bytes_read += read(fd, &memory[bytes_read], PPU_SIZE - bytes_read);
        if (bytes_read < 0) {
            perror("read");
            return -1;
        }
    } while(PPU_SIZE - bytes_read);

    return bytes_read;
}
