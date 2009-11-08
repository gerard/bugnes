#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "memhooks.h"

/* MEMORY HOOKS */

typedef enum {
    MEMHOOK_TYPE_READ,
    MEMHOOK_TYPE_WRITE,
    MEMHOOK_TYPE_MAX,
} mh_type_t;

/* XXX: Nasty #ifdefs to take out the debug thingies? Doesn't seem worth it
 * right now
 */
struct memhook {
    uint16_t addr_start;
    uint16_t addr_end;
    memhook_fun_t memhook_f;
    char *memhook_fun_name;
    struct memhook *next;
};

/* This is an array of linked list.  Each linked list is sorted by the address
 * of the mapping.  Besides, none of the mappings should be overlaped inside
 * the same list.
 */
static struct memhook *all_mh[MEMHOOK_TYPE_MAX];

static int insert(mh_type_t mh_type, memhook_fun_t mh_fun, char *mh_fun_name,
                  uint16_t addr_start, uint16_t addr_end)
{
    assert(addr_start < addr_end);

    /* We keep an iterator *behind* the mapping that we are checking to do the
     * insertion there.  A double linked list can be used instead, if this gets
     * too crazy
     * XXX: Linux include/list.h
     */
    struct memhook *mh_list = all_mh[mh_type];
    struct memhook *mh_list_prev = NULL;
    struct memhook *mh = (struct memhook *)malloc(sizeof(struct memhook));

    memset(mh, 0, sizeof(struct memhook));
    mh->addr_start = addr_start;
    mh->addr_end = addr_end;
    mh->memhook_f = mh_fun;

    /* We can stringify the function name via a macro and then have a nicer
     * output from memhook_dump().  Check definition in sfot_hooks.h.
     */
    if (mh_fun_name) {
        mh->memhook_fun_name = (char *)malloc(sizeof(char)*(strlen(mh_fun_name) + 1));
        strcpy(mh->memhook_fun_name, mh_fun_name);
    }

    /* There are 2 cases of overlapping, depending whether addr_start or
     * addr_end is inside of an existing mapping.  If both are inside, the
     * first one checked is addr_start.
     */
    while (mh_list) {
        if (addr_start < mh_list->addr_start) {
            if (addr_end >= mh_list->addr_start) {
                break;
            } else {
                /* Overlaps (at least) from the beginning of an exist mapping */
                return 1;
            }
        } else {
            if (addr_start < mh_list->addr_end) {
                /* Overlaps from the end of an existing mapping */
                return 1;
            }
        }

        mh_list_prev = mh_list;
        mh_list = mh_list->next;
    }

    if (mh_list_prev) {
        mh_list_prev->next = mh;
        mh->next = mh_list;
    } else all_mh[mh_type] = mh;

    return 0;
}

static memhook_fun_t check(mh_type_t mh_type, uint16_t addr)
{
    struct memhook *mh_list = all_mh[mh_type];

    while (mh_list) {
        if (mh_list->addr_start <= addr && addr < mh_list->addr_end) {
            return mh_list->memhook_f;
        } else mh_list = mh_list->next;
    }

    memhook_fun_t dummy;
    memset(&dummy, 0, sizeof(memhook_fun_t));

    return dummy;
}


/* INTERFACES for CPU users */

/* These are actually wrappers for the real insert:   we just set the hook
 * union and pass it to *the real thing* */
int sfot_memhook_i_insert_read(memhook_read_fun_t mh_fun, char *mh_fun_name,
                               uint16_t addr_start, uint16_t addr_end)
{
    memhook_fun_t hook = { .read = mh_fun };
    return insert(MEMHOOK_TYPE_READ, hook, mh_fun_name, addr_start, addr_end);
}

int sfot_memhook_i_insert_write(memhook_write_fun_t mh_fun, char *mh_fun_name,
                                uint16_t addr_start, uint16_t addr_end)
{
    memhook_fun_t hook = { .write = mh_fun };
    return insert(MEMHOOK_TYPE_WRITE, hook, mh_fun_name, addr_start, addr_end);
}

/* Debugging: Output all the memhooks registered */
void sfot_memhook_dump(char *buf)
{
    int count;
    int i;

    for (i = 0; i < MEMHOOK_TYPE_MAX; i++) {
        struct memhook *mh = all_mh[i];
        while (mh) {
            count = sprintf(buf, "MHTYPE %d [0x%04X-0x%04X] (%s)\n",
                    i, mh->addr_start, mh->addr_end, mh->memhook_fun_name);
            mh = mh->next;
            buf += count;
        }
    }
}

/* INTERFACES to self */
memhook_fun_t memhook_check_read(uint16_t addr) {
    return check(MEMHOOK_TYPE_READ, addr);
}

memhook_fun_t memhook_check_write(uint16_t addr) {
    return check(MEMHOOK_TYPE_WRITE, addr);
}
