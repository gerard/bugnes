#ifndef _MEMHOOKS_H_
#define _MEMHOOKS_H_

#include "../../../include/cpu/sfot_hooks_mem.h"

typedef union {
    memhook_read_fun_t read;
    memhook_write_fun_t write;
    memhook_transl_fun_t transl;
} memhook_fun_t;

memhook_fun_t memhook_check_read(uint16_t addr);
memhook_fun_t memhook_check_write(uint16_t addr);
memhook_fun_t memhook_check_transl(uint16_t addr);

#endif
