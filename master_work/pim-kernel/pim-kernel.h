/**
 * PIM kernel
 *
 * Version: 0:0.1
 */
#ifndef __PIM_KERNEL_H__
#define __PIM_KERNEL_H__

#include "regs.h"

/**
 * Command type
 */
enum command_type
{
    COMMAND_UNINIT = 0,
    COMMAND_INIT,
    COMMAND_NOP,
    COMMAND_NOVA_SEARCH_RBTREE,
    COMMAND_VFS_SEARCH_DCACHE,
    COMMAND_NOVA_FILE_R,
    COMMAND_NOVA_FILE_W,
    COMMAND_DONE,
};

void init_reg(void);

static inline void clflush(uint64_t addr, uint32_t size)
{
    REG_FLUSH_ADDR = addr;
    REG_FLUSH_SIZE = size;
}

/**
 * Input:
 *     REG 0: root physical address
 *     REG 1: hash key value
 * Output:
 *     REG 0: found (1: found, 0: not found)
 *     REG 1: curr physical address
 */
void kernel_nova_search_rbtree(void);

/**
 * Input:
 *     REG 0: parent dentry virtual address
 *     REG 1: struct hlist_bl_node first node virtual address
 *     REG 2: uint64_t hashlen
 *     REG 3: name string starting physical address
 * Output:
 *     REG 0: found (1: found, 0: not found)
 *     REG 1: dentry physical address
 *     REG 2: unsigned dentry sequence number
 */
void kernel_vfs_search_dcache(void);

/**
 * Input:
 *     REG 0: page global directory (PGD) entry value
 *     REG 1: destination virtual address (user space)
 *     REG 2: source physical address (kernel space)
 *     REG 3: size
 * Output:
 *     None
 *
 * Assume the page table has been pinned into memory
 */
void kernel_nova_file_r(void);

/**
 * Input:
 *     REG 0: page global directory (PGD) entry value
 *     REG 1: destination physical address (kernel space)
 *     REG 2: source virtual address (user space)
 *     REG 3: size
 * Output:
 *     None
 *
 * Assume the page table has been pinned into memory
 */
void kernel_nova_file_w(void);

#endif /* __PIM_KERNEL_H__ */
