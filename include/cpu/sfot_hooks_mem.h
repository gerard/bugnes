#ifndef _SFOT_HOOKS_MEM_H_
#define _SFOT_HOOKS_MEM_H_

#include <stdint.h>

/* Memory hook functions. */
typedef uint8_t (* memhook_read_fun_t)(uint16_t addr);
typedef uint8_t (* memhook_write_fun_t)(uint16_t addr, uint8_t write);
typedef uint16_t (* memhook_transl_fun_t)(uint16_t addr);

#define sfot_memhook_insert_read_simple(mh_f, where) \
        sfot_memhook_i_insert_read(mh_f, #mh_f, where, (where)+1)
#define sfot_memhook_insert_read(mh_f, start, end) \
        sfot_memhook_i_insert_read(mh_f, #mh_f, start, end)
int sfot_memhook_i_insert_read(memhook_read_fun_t mh_fun, char *mh_fun_name,
                               uint16_t addr_start, uint16_t addr_end);

#define sfot_memhook_insert_write_simple(mh_f, where) \
        sfot_memhook_i_insert_write(mh_f, #mh_f, where, (where)+1)
#define sfot_memhook_insert_write(mh_f, start, end) \
        sfot_memhook_i_insert_write(mh_f, #mh_f, start, end)
int sfot_memhook_i_insert_write(memhook_write_fun_t mh_fun, char *mh_fun_name,
                                uint16_t addr_start, uint16_t addr_end);

#define sfot_memhook_insert_transl_simple(mh_f, where) \
        sfot_memhook_i_insert_transl(mh_f, #mh_f, where, (where)+1)
#define sfot_memhook_insert_transl(mh_f, start, end) \
        sfot_memhook_i_insert_transl(mh_f, #mh_f, start, end)
int sfot_memhook_i_insert_transl(memhook_transl_fun_t mh_fun, char *mh_fun_name,
                                 uint16_t addr_start, uint16_t addr_end);

void sfot_memhook_dump(char *buf);

#endif
