#ifndef _SFOT_HOOKS_EXC_H_
#define _SFOT_HOOKS_EXC_H_

#include <stdint.h>

/* Exception hook functions.
 * This functions are executed on exceptions, like stack over/underflows and
 * unhandled BRK or interrupts.
 */
typedef void (* excepthook_fun_t)(const char *s);

typedef enum {
    EXCEPTHOOK_TYPE_BRK,
    EXCEPTHOOK_TYPE_MAX,
} eh_type_t;

void sfot_excepthook_insert(eh_type_t eh_type, excepthook_fun_t eh_fun);

#endif
