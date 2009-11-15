#ifndef _EXCEPTHOOKS_H_
#define _EXCEPTHOOKS_H_

#include "../../../include/cpu/sfot_hooks_exc.h"
void excepthook_check(eh_type_t eh, uint16_t exc_addr, const char *s);

#endif
