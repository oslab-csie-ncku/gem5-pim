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
    COMMAND_NOVA_DL_LOOKUP,
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
void kernel_nova_search_rbtree(volatile uint8_t cmd_index);

/**
 * Input:
 *     REG 0: struct dl_dir_lookup_entry first node virtual address
 *     REG 1: struct qstr virtual address
 *     REG 2: cred->fsuid
 *     REG 3: cred->fsgid
 * Output:
 *     REG 0: three possible return value:
 *              1. found, return ino number
 *              2. not found, ruturn 0 (there is no ino "0" inode in NOVA fs)
 *              3. permission check fail, return -1
 */
void kernel_dl_lookup_and_check(volatile uint8_t cmd_index);

#endif /* __PIM_KERNEL_H__ */
