#ifndef __LINUX_SEQLOCK_H__
#define __LINUX_SEQLOCK_H__

typedef struct seqcount
{
    unsigned sequence;
} seqcount_t;

static inline unsigned raw_seqcount_begin(volatile const seqcount_t *s)
{
    unsigned ret = s->sequence;
    return ret & ~1;
}

#endif /* __LINUX_SEQLOCK_H__ */
