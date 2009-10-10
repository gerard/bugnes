#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "cpu/sfot.h"

int step_cb(struct sfot_step_info *info)
{
    printf("%s\n", info->opcode_decoded);

    return 0;
}

uint8_t memhook_on_2002(uint16_t addr, uint8_t dummy)
{
    printf("Reading 0x%04X\n", addr);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return EXIT_FAILURE;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) return EXIT_FAILURE;

    char buf[1024];

    lseek(fd, 16, SEEK_CUR);

    sfot_load(fd, 0x8000, 0x8000);
    close(fd);

    sfot_install_step_cb(step_cb);
    sfot_memhook_insert(MEMHOOK_TYPE_READ, memhook_on_2002, 0x2002, 0x2003);
    sfot_memhook_dump(buf);
    printf(buf);

    sfot_poweron(SFOT_RUN_MAIN);

    return EXIT_SUCCESS;
}
