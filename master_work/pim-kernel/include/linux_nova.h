#ifndef __LINUX_NOVA_H__
#define __LINUX_NOVA_H__

#include "linux_rbtree.h"
#include "linux_types.h"

/**
 * A node in the RB tree representing a range of pages
 */
struct nova_range_node
{
    struct rb_node node;

    /**
      * not used, so you can replace it with a void pointer
      */
    // struct vm_area_struct *vma;
    void *vma;

    unsigned long mmap_entry;
    union
    {
        /* Block, inode */
        struct
        {
            unsigned long range_low;
            unsigned long range_high;
        };
        /* Dir node */
        struct
        {
            unsigned long hash;
            void *direntry;
        };
    };

    uint32_t csum; /* Protect vma, range low/high */
};

#endif /* __LINUX_NOVA_H__ */
