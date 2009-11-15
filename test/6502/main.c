/*
 * Tester for cpu/6502.  This file doesn't take into account any machine
 * specific behaviour, and just tests the implementation in cpu/6502, it
 * doesn't implement any kind of hook.  Separate tests for different machines
 * will hopefully be provided later.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "cpu/sfot.h"

#define BUG(s)              fprintf(stderr, "%s: this is not supposed to happen [%s():%d]\n", (s), __FUNCTION__, __LINE__)
#define TEST_FAIL(s1, s2)   fprintf(stderr, "\033[0;31mFAIL\033[m:\tExpected: %s\n           Got: %s\n", (s1), (s2));
#define TEST_SUCCESS(tv)    fprintf(stderr, "\033[0;32mSUCC\033[m:\t%s ran in %s secs\n", testfile_name, tv_print(tv))

struct timeval tv_startup;
struct timeval tv_teardown;
FILE *fd_test;
char *testfile_name;

/* Someday move this to another place */
struct timeval tv_sub(struct timeval tv1, struct timeval tv2)
{
    struct timeval tv;

    tv.tv_sec = tv1.tv_sec - tv2.tv_sec;
    tv.tv_usec = tv1.tv_usec - tv2.tv_usec;

    if (tv.tv_usec < 0) {
        tv.tv_sec  -= 1;
        tv.tv_usec += 1000000;
    }

    return tv;
}

/* User needs to free the returned string! */
char *tv_print(struct timeval tv)
{
    char *ret;
    asprintf(&ret, "%ld.%06ld", tv.tv_sec, tv.tv_usec);
    return ret;
}

/* We shouldn't rely on a string built in the cpu module, but construct it
 * independently here.  Anyway, formatting the debug line in the cpu module
 * is a bad idea.   Nevertheless, is what we have for now, and enables us to do
 * some automated testing, so that is what it is going to be until something
 * better comes.
 */
int step_cb(struct sfot_step_info *info)
{
    char test_line[1024];

    printf("%s\n", info->opcode_decoded);
    if (fd_test) {
        fscanf(fd_test, "%[^\n]\n", test_line);

        if (!strncmp(test_line, "END", 3)) {
            exit(EXIT_SUCCESS);
        }

        if (!strncmp(info->opcode_name, "BRK", 3)) {
            gettimeofday(&tv_teardown, NULL);
            TEST_SUCCESS(tv_sub(tv_teardown, tv_startup));
            exit(EXIT_SUCCESS);
        }

        if (strncmp(test_line, info->opcode_decoded, 1024)) {
            TEST_FAIL(test_line, info->opcode_decoded);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}


int main(int argc, char *argv[])
{
    if (argc < 2) return EXIT_FAILURE;
    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (argc == 3) {
        fd_test = fopen(argv[2], "r");
        testfile_name = strdup(argv[2]);
    }

    /* Calculate size */
    ssize_t romsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    /* Load, set the RESET location */
    sfot_load(fd, 0, romsize);
    sfot_set_reset(0x600);

    close(fd);

    /* Callbacks: Just the step callback is needed here */
    sfot_install_step_cb(step_cb);

    /* Crossing fingers... */
    gettimeofday(&tv_startup, NULL);
    sfot_poweron(SFOT_RUN_MAIN);

    BUG("END");
    return EXIT_FAILURE;
}
