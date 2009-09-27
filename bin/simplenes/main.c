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

int main(int argc, char *argv[])
{
    if (argc < 2) return EXIT_FAILURE;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) return EXIT_FAILURE;

    lseek(fd, 16, SEEK_CUR);

    sfot_load(fd, 0x8000, 0x8000);
    close(fd);

    sfot_install_step_cb(step_cb);
    sfot_poweron(SFOT_RUN_MAIN);

    return EXIT_SUCCESS;
}
