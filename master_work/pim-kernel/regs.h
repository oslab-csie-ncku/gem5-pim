#ifndef __REGS_H__
#define __REGS_H__

#include "linux_types.h"

/**
 * Register map
 */
#define REG_FLUSH_ADDR *((volatile uint64_t *)0x450000000)
#define REG_FLUSH_SIZE *((volatile uint32_t *)0x450000008)
#define REG_CMD        *((volatile uint8_t  *)0x45000000c)
#define REG_0          *((volatile uint64_t *)0x45000000d)
#define REG_1          *((volatile uint64_t *)0x450000015)
#define REG_2          *((volatile uint64_t *)0x45000001d)
#define REG_3          *((volatile uint64_t *)0x450000025)

#endif /* __REGS_H__ */
