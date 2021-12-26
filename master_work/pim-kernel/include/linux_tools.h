#ifndef __LINUX_TOOLS_H__
#define __LINUX_TOOLS_H__

#include "linux_types.h"

#define LINUX_PAGE_OFFSET ((uint64_t)0xffff880000000000)

#define offset_of(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                \
    volatile const typeof(((type *)0)->member) * __mptr = (ptr);   \
    (type *)((char *)__mptr - offset_of(type, member)); })

static inline uint64_t virt_to_phys(volatile const void *address)
{
    return (uint64_t)address - LINUX_PAGE_OFFSET;
}

static inline void *phys_to_virt(uint64_t address)
{
    return (void *)(address + LINUX_PAGE_OFFSET);
}

#endif /* __LINUX_TOOLS_H__ */
