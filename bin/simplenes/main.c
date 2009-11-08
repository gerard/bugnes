#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "ppu.h"
#include "cpu/sfot.h"

#define PPU_CR1_MAP         0x2000
#define PPU_CR2_MAP         0x2001
#define PPU_STATUS_MAP      0x2002
#define PPU_SPRITE_ADDR_MAP 0x2003
#define PPU_SPRITE_DATA_MAP 0x2004
#define PPU_SCR_SCOLL_MAP   0x2005
#define PPU_MEM_ADDR_MAP    0x2006
#define PPU_MEM_DATA_MAP    0x2007


int step_cb(struct sfot_step_info *info)
{
    printf("%s\n", info->opcode_decoded);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return EXIT_FAILURE;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) return EXIT_FAILURE;

    char buf[1024];

    lseek(fd, 16, SEEK_CUR);

    /* Should be noted that we just support games with 1 ROM bank and 1 VROM
     * bank; ie, the simplest games only.  Check the .nes header bytes 4 and 5.
     */
    sfot_load(fd, 0xC000, 0x4000);
    if (PPU_load(fd) < 0) {
        fprintf(stderr, "Error loading PPU Memory from ROM\n");
        return EXIT_FAILURE;
    }
    close(fd);

    sfot_install_step_cb(step_cb);

    sfot_memhook_insert_read_simple(PPU_cr1_read, PPU_CR1_MAP);
    sfot_memhook_insert_read_simple(PPU_cr2_read, PPU_CR2_MAP);
    sfot_memhook_insert_read_simple(PPU_status_read, PPU_STATUS_MAP);
    sfot_memhook_insert_read_simple(PPU_memdata_read, PPU_MEM_DATA_MAP);

    sfot_memhook_insert_write_simple(PPU_cr1_write, PPU_CR1_MAP);
    sfot_memhook_insert_write_simple(PPU_cr2_write, PPU_CR2_MAP);
    sfot_memhook_insert_write_simple(PPU_memaddr_write, PPU_MEM_ADDR_MAP);
    sfot_memhook_insert_write_simple(PPU_memdata_write, PPU_MEM_DATA_MAP);

    sfot_memhook_dump(buf);
    printf(buf);

    sfot_poweron(SFOT_RUN_MAIN);

    return EXIT_SUCCESS;
}
