/*
 * This file tries to emulate the 6502 system from this nice website:
 * o http://6502asm.com/
 *
 * The system has a framebuffer mapped starting at SCREEN_OFFSET with
 * a len of SCREEN_SIZE.
 * It also has 2 special memory mapped registers:
 *   - $FE: Random Generator
 *   - $FF: Last key pressed
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

#include "SDL.h"

#include "cpu/sfot.h"

#define SCREEN_HOOK_OFFSET  0x200
#define SCREEN_HOOK_SIZE    0x400
#define RANDOM_HOOK_OFFSET  0xFE
#define KEY_HOOK_OFFSET     0xFF

#define PIXEL_SIZE          8

#define BUG(s)              fprintf(stderr, "%s: this is not supposed to happen [%s():%d]\n", (s), __FUNCTION__, __LINE__)
#define EXCEPTION(s)        fprintf(stderr, "EXCEPTION: %s [%s():%d]\n", (s), __FUNCTION__, __LINE__)

SDL_Surface *screen;
SDL_TimerID refresh_timer;
static uint8_t framebuffer[0x400];
uint8_t keycode;
pthread_mutex_t keycode_m = PTHREAD_MUTEX_INITIALIZER;
pthread_t main_thread;

struct timeval tv_startup;
struct timeval tv_teardown;

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

int step_cb(struct sfot_step_info *info)
{
    printf("%s\n", info->opcode_decoded);

    /* We sleep here, because otherwise this goes too fast */
    /* XXX: Do some timing and have a steady framerate in every single cpu */
    usleep(1);

    return 0;
}

/* SDL Callback Update the screen */
uint32_t screen_update(uint32_t interval, void *dontcare) {
    uint32_t rgb;
    int i;

    for (i = 0; i < 0x400; i++) {
        switch (framebuffer[i]) {
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
        pixel.x = (i & 0x1f) * PIXEL_SIZE;
        pixel.y = (i >> 5) * PIXEL_SIZE;
        SDL_FillRect(screen, &pixel, rgb);
    }

    SDL_Flip(screen);

    /* SDL says to return this... whatever */
    return interval;
}

/* This is a hook from the CPU, so it should be fast.  Store the value and
 * leave the work for later (screen_update()).
 */
uint8_t screen_memhook(uint16_t addr, uint8_t color)
{
    return framebuffer[addr - SCREEN_HOOK_OFFSET] = color & 0xF;
}

uint8_t random_memhook(uint16_t addr)
{
    return random() & 0xFF;
}

uint8_t keycode_memhook(uint16_t addr)
{
    pthread_mutex_lock(&keycode_m);
    uint8_t ret = keycode;
    pthread_mutex_unlock(&keycode_m);

    return ret;
}

void excepthook(const char *s)
{
    EXCEPTION(s);

    /* Remove the timer and do the last refresh */
    SDL_RemoveTimer(refresh_timer);
    screen_update(1, NULL);

    gettimeofday(&tv_teardown, NULL);

    char *tv_printed = tv_print(tv_sub(tv_teardown, tv_startup));
    fprintf(stderr, "Total run time: %s\n", tv_printed);
    free(tv_printed);

    if (!pthread_equal(pthread_self(), main_thread)) {
        fprintf(stderr, "CPU thread done\n");
        pthread_exit(NULL);
    } else {
        exit(EXIT_SUCCESS);
    }
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

    /* XXX: This is not nice, but this is a fast way to know if the callbacks
     * are coming from the same thread or not
     */
    main_thread = pthread_self();

    /* SDL stuff */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
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
    sfot_set_reset(0x600);

    /* Walk the FB memory in the ROM */
    lseek(fd, SCREEN_HOOK_OFFSET, SEEK_SET);
    read(fd, fb_memory, SCREEN_HOOK_SIZE);
    for (i = 0; i < SCREEN_HOOK_SIZE; i++) {
        /* This speeds up the default case by avoiding SDL calls */
        if (!fb_memory[i]) continue;

        screen_memhook(SCREEN_HOOK_OFFSET + i, fb_memory[i]);
    }
    close(fd);

    /* Callbacks: Video hook, random hook and step by step callback */
    sfot_install_step_cb(step_cb);
    sfot_excepthook_insert(EXCEPTHOOK_TYPE_BRK, excepthook);
    sfot_memhook_insert_write(screen_memhook, SCREEN_HOOK_OFFSET,
                              SCREEN_HOOK_OFFSET+SCREEN_HOOK_SIZE);
    sfot_memhook_insert_read_simple(random_memhook, RANDOM_HOOK_OFFSET);
    sfot_memhook_insert_read_simple(keycode_memhook, KEY_HOOK_OFFSET);

    /* Check that the hook is there for good... */
    char buf[1024];
    sfot_memhook_dump(buf);
    printf(buf);

    /* Add timer for screen update 30 should get us around 30 FPS */
    refresh_timer = SDL_AddTimer(30, screen_update, NULL);

    /* Crossing fingers... */
    gettimeofday(&tv_startup, NULL);
    sfot_poweron(SFOT_RUN_THREADED);

    /* SFOT is running in a thread.  Now let's poll for events here */
    while (SDL_WaitEvent(&event)) {
        /* Magic to make CTRL-C work (just from console) */
        if (event.type == SDL_QUIT) return EXIT_SUCCESS;

        /* Magic combination to exit: LeftShift+Escape */
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE
                                      && (event.key.keysym.mod & KMOD_LSHIFT)) {
            return EXIT_SUCCESS;
        }

        /* If a key is released, we just clear keycode global and go on*/
        if (event.type == SDL_KEYUP) {
            pthread_mutex_lock(&keycode_m);
            keycode = 0;
            pthread_mutex_unlock(&keycode_m);
            continue;
        }

        /* Filter out non-keyboard events */
        if (event.type != SDL_KEYDOWN) continue;

        /* Filter out keypresses with modifiers, those should never reach the UI */
        if (event.key.keysym.mod != KMOD_NONE) continue;

        pthread_mutex_lock(&keycode_m);
        keycode = event.key.keysym.sym;
        pthread_mutex_unlock(&keycode_m);
    }

    BUG("END");
    return EXIT_FAILURE;
}
