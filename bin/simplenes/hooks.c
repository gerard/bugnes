#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "hooks.h"

/* All mirroring functions resort to this. it reads like this:
 * We have a address space from START to STOP, but the amount of physical
 * memory is SIZE.  Since the translation hook is executed before any other
 * hook, account that only the translated address will be accessed; ie, from
 * START to SIZE.
 */
uint16_t mirror(uint16_t addr, uint16_t start, uint16_t size)
{
    return (addr % size) + start;
}

uint16_t mirror_internal_ram(uint16_t addr)
{
    assert(addr < INTERNAL_RAM_MIRROR_STOP);

    return mirror(addr, INTERNAL_RAM_MIRROR_START, INTERNAL_RAM_MIRROR_STOP);
}
