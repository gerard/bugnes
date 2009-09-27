/*
 * This file tries to emulate the 6502 system from this nice website:
 * o http://6502asm.com/
 *
 * The system has a framebuffer mapped starting at SCREEN_OFFSET with
 * a len of SCREEN_SIZE.
 * Altough not implemented yet, it also has 2 special memory mapped registers:
 *   - $FE: Random Generator
 *   - $FF: Last key pressed
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "SDL.h"
#include "SDL_image.h"

#include "cpu/sfot.h"

#define SCREEN_OFFSET       0x200
#define SCREEN_SIZE         0x400

SDL_Surface *screen;

int step_cb(struct sfot_step_info *info)
{
    printf("%s\n", info->opcode_decoded);

    return 0;
}

uint8_t screen_memhook(uint16_t addr, uint8_t *color)
{
    uint32_t rgb = 0;

    switch (*color & 0xF) {
    case 0x0: rgb = SDL_MapRGB(screen->format,    0,    0,    0); break;
    case 0x1: rgb = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF); break;
    case 0x2: rgb = SDL_MapRGB(screen->format, 0x88,    0,    0); break;
    case 0x3: rgb = SDL_MapRGB(screen->format, 0xAA, 0xFF, 0xEE); break;
    case 0x4: rgb = SDL_MapRGB(screen->format, 0xCC, 0x44, 0xCC); break;
    case 0x5: rgb = SDL_MapRGB(screen->format,    0, 0xCC, 0x55); break;
    case 0x6: rgb = SDL_MapRGB(screen->format,    0,    0, 0xAA); break;
    case 0x7: rgb = SDL_MapRGB(screen->format, 0xEE, 0xEE, 0x77); break;
    case 0x8: rgb = SDL_MapRGB(screen->format, 0xDD, 0x88, 0x55); break;
    case 0x9: rgb = SDL_MapRGB(screen->format, 0x66, 0x44,    0); break;
    case 0xA: rgb = SDL_MapRGB(screen->format, 0xFF, 0x77, 0x77); break;
    case 0xB: rgb = SDL_MapRGB(screen->format, 0x33, 0x33, 0x33); break;
    case 0xC: rgb = SDL_MapRGB(screen->format, 0x77, 0x77, 0x77); break;
    case 0xD: rgb = SDL_MapRGB(screen->format, 0xAA, 0xFF, 0x77); break;
    case 0xE: rgb = SDL_MapRGB(screen->format, 0x00, 0x88, 0xFF); break;
    case 0xF: rgb = SDL_MapRGB(screen->format, 0xBB, 0xBB, 0xBB); break;
    }

    SDL_LockSurface(screen);
    ((uint8_t *)screen->pixels)[addr - SCREEN_OFFSET] = (uint8_t)rgb;
    SDL_UnlockSurface(screen);

    SDL_Flip(screen);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return EXIT_FAILURE;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) return EXIT_FAILURE;

    /* SDL stuff */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    if(!(screen = SDL_SetVideoMode(32, 32, 8, SDL_DOUBLEBUF | SDL_HWSURFACE))) {
        fprintf(stderr, "Couldn't set Video Mode: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* Calculate size */
    ssize_t romsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    /* Load, set the RESET location */
    sfot_load(fd, 0, romsize);
    sfot_set_reset(0xC000);
    close(fd);

    /* Callbacks: Video hook and step by step callback*/
    sfot_install_step_cb(step_cb);
    sfot_memhook_insert(MEMHOOK_TYPE_WRITE, screen_memhook,
                        SCREEN_OFFSET, SCREEN_OFFSET+SCREEN_SIZE);

    /* Check that the hook is there for good... */
    char buf[1024];
    sfot_memhook_dump(buf);
    printf(buf);

    /* Crossing fingers... */
    sfot_poweron(SFOT_RUN_MAIN);

    return EXIT_SUCCESS;
}
