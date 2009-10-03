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
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"

#include "cpu/sfot.h"

#define SCREEN_HOOK_OFFSET  0x200
#define SCREEN_HOOK_SIZE    0x400
#define RANDOM_HOOK_OFFSET  0xFE
#define RANDOM_HOOK_SIZE    0x1

#define PIXEL_SIZE          8

#define BUG(s)                 fprintf(stderr, "%s: this is not supposed to happen: %s:%d\n", (s), __FUNCTION__, __LINE__)

SDL_Surface *screen;

int step_cb(struct sfot_step_info *info)
{
    printf("%s\n", info->opcode_decoded);

    /* For some reason, if we start flooding the stdout, stdin looses all
     * tracks of what "being responsive" means.
     * We introduce a small delay here so SDL_WaitEvents gets a chance to see
     * the keypresses.  Not nice for performance; anyway, you shouldn't be
     * using this callback at all if you want performace.
     */
    usleep(10);

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

    SDL_Rect pixel;
    pixel.w = pixel.h = PIXEL_SIZE;
    pixel.x = ((addr - SCREEN_HOOK_OFFSET) & 0x1f) * PIXEL_SIZE;
    pixel.y = ((addr - SCREEN_HOOK_OFFSET) >> 5) * PIXEL_SIZE;
    SDL_FillRect(screen, &pixel, rgb);

    SDL_Flip(screen);

    return 0;
}

uint8_t random_memhook(uint16_t addr, uint8_t *dummy)
{
    return random() & 0xFF;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return EXIT_FAILURE;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) return EXIT_FAILURE;
    SDL_Event event;
    uint8_t fb_memory[SCREEN_HOOK_SIZE];
    uint16_t i;

    /* Initialize randomness */
    srandom(time(NULL));

    /* SDL stuff */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    if(!(screen = SDL_SetVideoMode(32*PIXEL_SIZE, 32*PIXEL_SIZE, 8,
                                   SDL_DOUBLEBUF | SDL_HWSURFACE))) {
        fprintf(stderr, "Couldn't set Video Mode: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* Calculate size */
    ssize_t romsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    /* Load, set the RESET location */
    sfot_load(fd, 0, romsize);
    sfot_set_reset(0xC000);

    /* Walk the FB memory in the ROM */
    lseek(fd, SCREEN_HOOK_OFFSET, SEEK_SET);
    read(fd, fb_memory, SCREEN_HOOK_SIZE);
    for (i = 0; i < SCREEN_HOOK_SIZE; i++) {
        screen_memhook(SCREEN_HOOK_OFFSET + i, &fb_memory[i]);
    }
    close(fd);

    /* Callbacks: Video hook, random hook and step by step callback */
    sfot_install_step_cb(step_cb);
    sfot_memhook_insert(MEMHOOK_TYPE_WRITE, screen_memhook,
                        SCREEN_HOOK_OFFSET, SCREEN_HOOK_OFFSET+SCREEN_HOOK_SIZE);
    sfot_memhook_insert(MEMHOOK_TYPE_READ, random_memhook,
                        RANDOM_HOOK_OFFSET, RANDOM_HOOK_OFFSET+RANDOM_HOOK_SIZE);

    /* Check that the hook is there for good... */
    char buf[1024];
    sfot_memhook_dump(buf);
    printf(buf);

    /* Crossing fingers... */
    sfot_poweron(SFOT_RUN_THREADED);

    /* SFOT is running in a thread.  Now let's poll for events here */
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            return EXIT_SUCCESS;
        }
    }

    BUG("END");
    return EXIT_FAILURE;
}
