#ifndef __LINUX_PGTABLE_H__
#define __LINUX_PGTABLE_H__

#include "linux_stddef.h"
#include "linux_types.h"

#define PGDIR_SHIFT 39
#define PUD_SHIFT   30
#define PMD_SHIFT   21
#define PAGE_SHIFT  12

#define PUD_PAGE_SIZE ((uint64_t)1 << PUD_SHIFT)
#define PMD_PAGE_SIZE ((uint64_t)1 << PMD_SHIFT)
#define PAGE_SIZE     ((uint64_t)1 << PAGE_SHIFT)

#define PUD_PAGE_MASK (~(PUD_PAGE_SIZE-1))
#define PMD_PAGE_MASK (~(PMD_PAGE_SIZE-1))
#define PAGE_MASK     (~(PAGE_SIZE-1))

#define PTRS_PER_PGD 512
#define PTRS_PER_PUD 512
#define PTRS_PER_PMD 512
#define PTRS_PER_PTE 512

#define PTE_PFN_MASK   ((uint64_t)0xffffffffff000)
#define PTE_FLAGS_MASK (~PTE_PFN_MASK)

#define _PAGE_BIT_PRESENT 0 /* is present */
#define _PAGE_BIT_GLOBAL  8 /* Global TLB entry PPro+ */
#define _PAGE_BIT_PROTNONE _PAGE_BIT_GLOBAL

#define _PAGE_PRESENT  ((uint64_t)1 << _PAGE_BIT_PRESENT)
#define _PAGE_PROTNONE ((uint64_t)1 << _PAGE_BIT_PROTNONE)

#define _PAGE_KNL_ERRATUM_MASK 0x60
#define _PAGE_USER             0x4
#define _KERNPG_TABLE          0x63

typedef uint64_t pgdval_t;
typedef uint64_t pudval_t;
typedef uint64_t pmdval_t;
typedef uint64_t pteval_t;

typedef struct
{
    pgdval_t pgdval; } pgd_t;
typedef struct
{
    pudval_t pudval; } pud_t;
typedef struct
{
    pmdval_t pmdval; } pmd_t;
typedef struct
{
    pteval_t pteval; } pte_t;

/**
 * pud utilities
 */
static inline uint64_t pud_index(uint64_t vaddr)
{
    return (vaddr >> PUD_SHIFT) & (PTRS_PER_PUD - 1);
}

static inline pud_t *pud_offset(pgdval_t pgdval, uint64_t vaddr)
{
    return (pud_t *)(pgdval & PTE_PFN_MASK) + pud_index(vaddr);
}

static inline int pud_none(pudval_t pudval)
{
    return (pudval & ~_PAGE_KNL_ERRATUM_MASK) == 0;
}

static inline pudval_t pud_flags(pudval_t pudval)
{
    return pudval & ~PTE_PFN_MASK;
}

static inline int pud_bad(pudval_t pudval)
{
    return (pud_flags(pudval) & ~(_KERNPG_TABLE | _PAGE_USER)) != 0;
}

/**
 * pmd utilities
 */
static inline uint64_t pmd_index(uint64_t vaddr)
{
    return (vaddr >> PMD_SHIFT) & (PTRS_PER_PMD - 1);
}

static inline pmd_t *pmd_offset(pudval_t pudval, uint64_t vaddr)
{
    return (pmd_t *)(pudval & PTE_PFN_MASK) + pmd_index(vaddr);
}

static inline int pmd_none(pmdval_t pmdval)
{
    return (pmdval & ~_PAGE_KNL_ERRATUM_MASK) == 0;
}

static inline pmdval_t pmd_flags(pmdval_t pmdval)
{
    return pmdval & ~PTE_PFN_MASK;
}

static inline int pmd_bad(pmdval_t pmdval)
{
    return (pmd_flags(pmdval) & ~_PAGE_USER) != _KERNPG_TABLE;
}

/**
 * pte utilities
 */
static inline uint64_t pte_index(uint64_t vaddr)
{
    return (vaddr >> PAGE_SHIFT) & (PTRS_PER_PTE - 1);
}

static inline pte_t *pte_offset(pmdval_t pmdval, uint64_t vaddr)
{
    return (pte_t *)(pmdval & PTE_PFN_MASK) + pte_index(vaddr);
}

static inline int pte_none(pteval_t pteval)
{
    return (pteval & ~_PAGE_KNL_ERRATUM_MASK) == 0;
}

static inline pteval_t pte_flags(pteval_t pteval)
{
    return pteval & PTE_FLAGS_MASK;
}

static inline int pte_present(pteval_t pteval)
{
    return pte_flags(pteval) & (_PAGE_PRESENT | _PAGE_PROTNONE);
}

static inline uint64_t pteval_to_phys(pteval_t pteval, uint64_t vaddr)
{
    return (pteval & PTE_PFN_MASK) + (vaddr & ~PAGE_MASK);
}

/**
 * Page table walk
 *
 * Assume the page table has been pinned into memory
 */
static inline uint64_t user_virt_to_phys(pgdval_t pgdval, uint64_t vaddr,
                                         void (*clflush)(uint64_t, uint32_t))
{
    volatile pud_t *pud = NULL;
    volatile pmd_t *pmd = NULL;
    volatile pte_t *pte = NULL;
    pudval_t pudval = 0;
    pmdval_t pmdval = 0;
    pteval_t pteval = 0;

    pud = pud_offset(pgdval, vaddr);
    clflush((uint64_t)pud, sizeof(pud_t));
    pudval = pud->pudval;
    //if (pud_none(pudval) || pud_bad(pudval))
    //    goto no_page_table;

    pmd = pmd_offset(pudval, vaddr);
    clflush((uint64_t)pmd, sizeof(pmd_t));
    pmdval = pmd->pmdval;
    //if (pmd_none(pmdval) || pmd_bad(pmdval))
    //    goto no_page_table;

    pte = pte_offset(pmdval, vaddr);
    clflush((uint64_t)pte, sizeof(pte_t));
    pteval = pte->pteval;
    //if (pte_none(pteval) || !pte_present(pteval))
    //    goto no_page;

    return pteval_to_phys(pteval, vaddr);

//no_page:
//no_page_table:
//    err;
//    return 0;
}

#endif /* __LINUX_PGTABLE_H__ */
