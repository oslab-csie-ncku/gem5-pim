#ifndef __LINUX_DCACHE_H__
#define __LINUX_DCACHE_H__

#include "linux_list_bl.h"
#include "linux_seqlock.h"
#include "linux_types.h"

#define bytemask_from_count(cnt) (~(~0ul << (cnt)*8))

struct qstr
{	union {
		struct {
			uint32_t len;
            uint32_t hash;
		};
		uint64_t hash_len;
	};
    const unsigned char *name;
};

struct dentry_lookup
{
    // contain lookup touched fields only
    /* RCU lookup touched fields */
    unsigned int d_flags;        /* protected by d_lock */
    seqcount_t d_seq;            /* per dentry seqlock */
    struct hlist_bl_node d_hash; /* lookup hash list */
    struct dentry_lookup *d_parent;     /* parent directory */
    struct qstr d_name;
};

static inline int d_unhashed(volatile const struct dentry_lookup *dentry)
{
    return hlist_bl_unhashed(&dentry->d_hash);
}

static inline int dentry_string_cmp(volatile const unsigned char *cs,
                                    volatile const unsigned char *ct,
                                    unsigned tcount)
{
    unsigned long a, b, mask;

    for (;;) {
        a = *(unsigned long *)cs;
        b = *(unsigned long *)ct;
        if (tcount < sizeof(unsigned long))
            break;
        if (a != b)
            return 1;
        cs += sizeof(unsigned long);
        ct += sizeof(unsigned long);
        tcount -= sizeof(unsigned long);
        if (!tcount)
            return 0;
    }
    mask = bytemask_from_count(tcount);
    return !!((a ^ b) & mask);
}

#endif /* __LINUX_DCACHE_H__ */
