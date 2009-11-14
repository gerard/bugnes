#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "opcodes.h"
#include "addr.h"
#include "core.h"
#include "disas.h"

#include "cpu/sfot.h"

static sfot_step_cb step_cb;
pthread_t thr;

static void collect_step_info(struct sfot_step_info *info)
{
    int i;

    memset(info, 0, sizeof(struct sfot_step_info));

    strncpy(info->opcode_name, opcode_get_name(), 3);
    for (i = 0; i < SFOT_REG_MAX; i++) {
        info->registers[i] = GET_SREG(i);
    }
    info->reg_pc = GET_PC();
    info->total_cycles = 0;         /* FIXME */
    info->addr_mode = opcode_get_addr_mode();
    info->opcode_par_n = opcode_get_params(&(info->opcode_par[0]),
                                           &(info->opcode_par[1]));

    disassemble_opcode(info);
}

static void *cpu_thread(void *arg)
{
    struct sfot_step_info step_info;

    if (pthread_equal(pthread_self(), thr)) {
        /* Running in a thread: close stdin */
        /* XXX: Probably we should also close stdout/stderr and comunicat via hooks */
        close(0);
    }

    while (1) {
        opcode_fetch();
        opcode_exec();
        
        if (!step_cb) continue;
        
        collect_step_info(&step_info);
        if (step_cb(&step_info)) break;
    }

    return NULL;
}

int sfot_load(int fd, uint16_t offset, uint16_t len)
{
    uint8_t buf[0x10000];
    int bytes_read = 0;
    int i;

    if (fd < 0) return -1;
    if (((uint32_t)offset + (uint32_t)len) > 0x10000) return -1;

    do {
        bytes_read += read(fd, &buf[offset + bytes_read], len - bytes_read);
        if (bytes_read < 0) return -1;
    } while(len - bytes_read);

    for (i = offset; i < offset+len; i++) {
        MEM_WRITE(i, buf[i]);
    }

    assert(bytes_read == len);
    return bytes_read;
}

int sfot_load_stream(const uint8_t *s, uint16_t offset, uint16_t len)
{
    int i;

    for (i = offset; i < offset+len; i++) {
        MEM_WRITE(i, s[i-offset]);
    }

    return len;
}

void sfot_set_reset(uint16_t val)
{
    MEM_WRITE16(CPU_ADDR_RESET, val);
}

sfot_step_cb sfot_install_step_cb(sfot_step_cb cb)
{
    sfot_step_cb old_cb = step_cb;

    step_cb = cb;

    return old_cb;
}

void sfot_poweron(sfot_running_mode runmode)
{
    opcode_fetch_start(MEM_READ16(CPU_ADDR_RESET));

    switch(runmode) {
    case SFOT_RUN_MAIN:
        cpu_thread(NULL);
        break;
    case SFOT_RUN_THREADED:
        pthread_create(&thr, NULL, cpu_thread, NULL);
        pthread_detach(thr);
        break;
    }
}
