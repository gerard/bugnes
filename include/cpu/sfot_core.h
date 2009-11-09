#ifndef _SFOT_H_
#define _SFOT_H_

#include <stdint.h>

typedef enum {
    SFOT_RUN_MAIN,
    SFOT_RUN_THREADED
} sfot_running_mode;

/* Right now, this 2 enums have to mach to the in-core definitions, at least
 * up to MAX.
 */
typedef enum {
    SFOT_REG_INVALID = -1,
    SFOT_REG_SP,
    SFOT_REG_ACC,
    SFOT_REG_X,
    SFOT_REG_Y,
    SFOT_REG_CPU,
    SFOT_REG_MAX
} sfot_short_reg_t;

typedef enum {
    SFOT_A_IMP,
    SFOT_A_ACC,
    SFOT_A_IMM,
    SFOT_A_ZPA,
    SFOT_A_ZPX,
    SFOT_A_ZPY,
    SFOT_A_REL,
    SFOT_A_ABS,
    SFOT_A_ABX,
    SFOT_A_ABY,
    SFOT_A_IND,
    SFOT_A_INX,
    SFOT_A_INY,
    SFOT_A_MAX
} sfot_addr_mode_t;

struct sfot_step_info {
    uint64_t total_cycles;
    uint16_t reg_pc;
    uint8_t registers[SFOT_REG_MAX];
    char opcode_name[4];
    sfot_addr_mode_t addr_mode;
    int opcode_par_n;
    uint8_t opcode_par[2];
    char opcode_decoded[128];
};

typedef int (*sfot_step_cb)(struct sfot_step_info *info);
sfot_step_cb sfot_install_step_cb(sfot_step_cb step_cb);

void sfot_set_reset(uint16_t val);

int sfot_load(int fd, uint16_t offset, uint16_t len);
int sfot_load_stream(uint8_t *s, uint16_t offset, uint16_t len);
void sfot_poweron();

#endif
