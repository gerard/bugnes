#include <stdio.h>
#include "excepthooks.h"

static excepthook_fun_t eh_fun_v[EXCEPTHOOK_TYPE_MAX];

/* Getter: Calling interface for the CPU */
void excepthook_check(eh_type_t eh_type, uint16_t exc_addr, const char *s)
{
    excepthook_fun_t hook = eh_fun_v[eh_type];
    if (hook) hook(exc_addr, s);
}

/* Setter: Hook insertion for users (machines) */
void sfot_excepthook_insert(eh_type_t eh_type, excepthook_fun_t eh_fun)
{
    eh_fun_v[eh_type] = eh_fun;
}

void sfot_excepthook_clear()
{
    int i;

    for (i = 0; i < EXCEPTHOOK_TYPE_MAX; i++) {
        eh_fun_v[i] = NULL;
    }
}
