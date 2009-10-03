#ifndef _SFOT_HOOKS_H_
#define _SFOT_HOOKS_H_

#include <stdint.h>

typedef enum {
    MEMHOOK_TYPE_READ,
    MEMHOOK_TYPE_WRITE,
    MEMHOOK_TYPE_MAX,
} mh_type_t;

typedef enum {
    EXCEPTHOOK_TYPE_BRK,
    EXCEPTHOOK_TYPE_MAX,
} eh_type_t;

/* Memory hook function.
 * Readers shouldn't expect anything but NULL on the "write" parameter.
 * Instead, they have to return the value to be given to userspace.
 *
 * Writers, instead just take a value from userspace.  If they wish to do
 * something with the actual memory location, they can access it through the
 * "write" parameter.
 */
typedef uint8_t (* memhook_fun_t)(uint16_t addr, uint8_t *write);

#define sfot_memhook_insert(type, mh_f, start, end) sfot_memhook_i_insert(type, mh_f, #mh_f, start, end)
int sfot_memhook_i_insert(mh_type_t mh_type, memhook_fun_t mh_fun, char *mh_fun_name,
                     uint16_t addr_start, uint16_t addr_end);
void sfot_memhook_dump(char *buf);


/* Exception hook functions.
 * This functions are executed on exceptions, like stack over/underflows and
 * unhandled BRK or interrupts
 */
typedef void (* excepthook_fun_t)(const char *s);

void sfot_excepthook_insert(eh_type_t eh_type, excepthook_fun_t eh_fun);

#endif
