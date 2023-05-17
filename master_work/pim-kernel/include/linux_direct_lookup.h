#ifndef __LINUX_DIRECT_LOOKUP_H__
#define __LINUX_DIRECT_LOOKUP_H__

#include "linux_list.h"
#include "linux_types.h"

typedef struct {
	uint32_t val;
} kuid_t;


typedef struct {
	uint32_t val;
} kgid_t;

enum dl_reachable_type{
    invalid, other, gid, not_uid,
    uid, not_gid, uid_or_gid, valid
};

struct dl_reachable_entry{
    enum dl_reachable_type type;
    kuid_t uid;
    kgid_t gid;
};

/*
 * Structure of full inode search permission for a directory.
 * Check this when the system supports direct directory lookup mechanism
 */
struct dl_reachable_set{
    uint16_t num_entries;
    struct dl_reachable_entry entries[];
};

struct dl_dir_lookup_entry {
    uint64_t identity_hash;
    uint64_t ino;
    struct dl_reachable_set *rset;
    struct hlist_node dl_hash;
} __attribute((__packed__));

#endif /* __LINUX_DIRECT_LOOKUP_H__ */