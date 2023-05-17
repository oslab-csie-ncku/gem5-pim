#ifndef __REGS_H__
#define __REGS_H__

#include "linux_types.h"

/* pim_system: 0 */
#ifdef DPIM0
#define SPM_START       (0x450000000)
#define REG_BASE        (0x45000000c)
#endif
/* pim_system: 1 */
#ifdef DPIM1
#define SPM_START       (0x450001000)
#define REG_BASE        (0x45000100c)
#endif
/* pim_system: 2 */
#ifdef DPIM2
#define SPM_START       (0x450002000)
#define REG_BASE        (0x45000200c)
#endif
/* pim_system: 3 */
#ifdef DPIM3
#define SPM_START       (0x450003000)
#define REG_BASE        (0x45000300c)
#endif
#define SPM_SIZE        ((1) << (12UL)) /* 4096 bytes*/

/* Register offset for each job relative to REG_0 */
#define REG_FLUSH_SIZE_OFFSET   0x08
#define REG_CMD_OFFSET	        0x0c
#define REG_0_OFFSET		    0x0d
#define REG_1_OFFSET		    0x15
#define REG_2_OFFSET		    0x1d
#define REG_3_OFFSET		    0x25
#define PIM_JOB_SIZE            33
#define MAX_JOB_NUM		        ((SPM_SIZE - REG_CMD_OFFSET) / PIM_JOB_SIZE)

/**
 * Register map(fixed)
 */
#define REG_FLUSH_ADDR *((volatile uint64_t *)(SPM_START))
#define REG_FLUSH_SIZE *((volatile uint32_t *)(SPM_START + REG_FLUSH_SIZE_OFFSET))
#define REG_FIRST_CMD  *((volatile uint8_t  *)(SPM_START + REG_CMD_OFFSET))

 
#define REG_CMD(N)     *((volatile uint8_t  *)(SPM_START + ((N) * PIM_JOB_SIZE) + REG_CMD_OFFSET)) /* 1 bytes */
#define REG_0(N)       *((volatile uint64_t *)(SPM_START + ((N) * PIM_JOB_SIZE) + REG_0_OFFSET)) /* 8 bytes */
#define REG_1(N)       *((volatile uint64_t *)(SPM_START + ((N) * PIM_JOB_SIZE) + REG_1_OFFSET)) /* 8 bytes */
#define REG_2(N)       *((volatile uint64_t *)(SPM_START + ((N) * PIM_JOB_SIZE) + REG_2_OFFSET)) /* 8 bytes */
#define REG_3(N)       *((volatile uint64_t *)(SPM_START + ((N) * PIM_JOB_SIZE) + REG_3_OFFSET)) /* 8 bytes */

/**
 * The size of each register
 */
#define REG_CMD_SIZE	1
#define REG_0_SIZE		8
#define REG_1_SIZE		8
#define REG_2_SIZE		8
#define REG_3_SIZE		8

#endif /* __REGS_H__ */
