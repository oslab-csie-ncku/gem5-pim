#include "linux_dcache.h"
#include "linux_list_bl.h"
#include "linux_nova.h"
#include "linux_pgtable.h"
#include "linux_rbtree.h"
#include "linux_seqlock.h"
#include "linux_stddef.h"
#include "linux_string.h"
#include "linux_stringhash.h"
#include "linux_tools.h"
#include "linux_types.h"
#include "pim-kernel.h"

void init_reg(void)
{
    REG_FLUSH_ADDR = 0;
    REG_FLUSH_SIZE = 0;
    REG_CMD = COMMAND_UNINIT;
    REG_0 = 0;
    REG_1 = 0;
    REG_2 = 0;
    REG_3 = 0;
}

void kernel_nova_search_rbtree(void)
{
    volatile const struct rb_root *tree = (struct rb_root *)REG_0;
    const uint64_t hash_key = REG_1;
    volatile struct rb_node *_node = NULL;
    volatile struct nova_range_node *curr = NULL;

    clflush((uint64_t)tree, sizeof(struct rb_root));

    for (_node = tree->rb_node;
         _node && ({_node = (struct rb_node *)virt_to_phys(_node);
                    curr = container_of(_node, struct nova_range_node, node);
                    clflush((uint64_t)curr, sizeof(struct nova_range_node));
                    1;});
         _node = hash_key < curr->hash ? _node->rb_left : _node->rb_right) {
        if (hash_key == curr->hash) {
            REG_0 = 1;
            REG_1 = (uint64_t)curr;
            return;
        }
    }

    REG_0 = 0;
}

void kernel_vfs_search_dcache(void)
{
    const uint64_t parent_dentry_virt_addr = REG_0;
    volatile struct hlist_bl_node *node = (struct hlist_bl_node *)REG_1;
    const uint64_t hashlen = REG_2;
    volatile const unsigned char *str = (const unsigned char *)REG_3;
    volatile struct dentry_lookup *dentry = NULL;

    for (;
         node && ({node = (struct hlist_bl_node *)virt_to_phys(node);
                   dentry = hlist_bl_entry(node, struct dentry_lookup, d_hash);
                   clflush((uint64_t)dentry, sizeof(struct dentry_lookup));
                   1;});
         node = node->next) {
        const unsigned seq = raw_seqcount_begin(&dentry->d_seq);

        if ((uint64_t)dentry->d_parent != parent_dentry_virt_addr)
            continue;

        if (d_unhashed(dentry))
            continue;

        if (dentry->d_name.hash_len != hashlen)
            continue;

        volatile const unsigned char *dentry_name = (const unsigned char *)
            virt_to_phys(dentry->d_name.name);
        clflush((uint64_t)dentry_name, hashlen_len(hashlen));
        clflush((uint64_t)str, hashlen_len(hashlen));
        if (dentry_string_cmp(dentry_name, str, hashlen_len(hashlen)) != 0)
            continue;

        REG_0 = 1;
        REG_1 = (uint64_t)dentry;
        REG_2 = (uint64_t)seq;
        return;
    }

    REG_0 = 0;
}

void kernel_nova_file_r(void)
{
    pgdval_t pgdval = REG_0;
    uint64_t dst_virt_addr = REG_1;
    uint64_t src_phys_addr = REG_2;
    uint64_t size = REG_3;

    while (size) {
        uint16_t round_max = PAGE_SIZE - (dst_virt_addr & ~PAGE_MASK);
        uint16_t round_size;
        uint64_t dst_phys_addr = user_virt_to_phys(pgdval, dst_virt_addr,
                                                   clflush);

        if (size <= round_max)
            round_size = size;
        else
            round_size = round_max;
        size -= round_size;

        clflush(dst_phys_addr, round_size);
        memcpy_v((void *)dst_phys_addr, (void *)src_phys_addr, round_size);

        dst_virt_addr += round_size;
        src_phys_addr += round_size;
    }
}

void kernel_nova_file_w(void)
{
    pgdval_t pgdval = REG_0;
    uint64_t dst_phys_addr = REG_1;
    uint64_t src_virt_addr = REG_2;
    uint64_t size = REG_3;

    while (size) {
        uint16_t round_max = PAGE_SIZE - (src_virt_addr & ~PAGE_MASK);
        uint16_t round_size;
        uint64_t src_phys_addr = user_virt_to_phys(pgdval, src_virt_addr,
                                                   clflush);

        if (size <= round_max)
            round_size = size;
        else
            round_size = round_max;
        size -= round_size;

        clflush(src_phys_addr, round_size);
        memcpy_v((void *)dst_phys_addr, (void *)src_phys_addr, round_size);

        dst_phys_addr += round_size;
        src_virt_addr += round_size;
    }
}

int pim_start()
{
    init_reg();

    // Waiting for initialization info from OS
    while (REG_CMD == COMMAND_UNINIT);

    // Start working
    while (1) {
        const uint8_t cmd = REG_CMD;

        if (cmd >= COMMAND_NOVA_SEARCH_RBTREE && cmd <= COMMAND_NOVA_FILE_W) {
            switch (cmd) {
            case COMMAND_NOVA_SEARCH_RBTREE:
                kernel_nova_search_rbtree();
                break;
            case COMMAND_VFS_SEARCH_DCACHE:
                kernel_vfs_search_dcache();
                break;
            case COMMAND_NOVA_FILE_R:
                kernel_nova_file_r();
                break;
            case COMMAND_NOVA_FILE_W:
                kernel_nova_file_w();
                break;
            default:
                ; // do nothing
            }

            REG_CMD = COMMAND_DONE;
        }
    }

    return 0;
}
