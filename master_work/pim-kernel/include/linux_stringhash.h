#ifndef __LINUX_STRINGHASH_H__
#define __LINUX_STRINGHASH_H__

#include "linux_types.h"

/*
 * A hash_len is a u64 with the hash of a string in the low
 * half and the length in the high half.
 */
#define hashlen_hash(hashlen) ((uint32_t)(hashlen))
#define hashlen_len(hashlen)  ((uint32_t)((hashlen) >> 32))
#define hashlen_create(hash, len) ((uint64_t)(len)<<32 | (uint32_t)(hash))

#endif /* __LINUX_STRINGHASH_H__ */
