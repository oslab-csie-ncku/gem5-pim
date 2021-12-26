#ifndef __LINUX_STRING_H__
#define __LINUX_STRING_H__

static inline void memcpy_v(volatile void *p, volatile const void *q,
                          unsigned long size)
{
    for (;;) {
        if (size < sizeof(unsigned long))
            break;

        *(volatile unsigned long *)p = *(volatile const unsigned long *)q;
        p += sizeof(unsigned long);
        q += sizeof(unsigned long);
        size -= sizeof(unsigned long);
        if (!size)
            return;
    }

    for (;;) {
        *(volatile unsigned char *)p = *(volatile const unsigned char *)q;
        p += sizeof(unsigned char);
        q += sizeof(unsigned char);
        size -= sizeof(unsigned char);
        if (!size)
            return;
    }
}

#endif /* __LINUX_STRING_H__ */
