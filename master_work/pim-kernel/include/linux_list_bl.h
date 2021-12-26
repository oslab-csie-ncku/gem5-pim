#ifndef __LINUX_LIST_BL_H__
#define __LINUX_LIST_BL_H__

#include "linux_tools.h"
#include "linux_types.h"

#define hlist_bl_entry(ptr, type, member) container_of(ptr, type, member)

struct hlist_bl_head
{
    struct hlist_bl_node *first;
};

struct hlist_bl_node
{
    struct hlist_bl_node *next, **pprev;
};

static inline bool hlist_bl_unhashed(volatile const struct hlist_bl_node *h)
{
    return !h->pprev;
}

#endif /* __LINUX_LIST_BL_H__ */
