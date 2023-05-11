#ifndef OVERLAYFS
#define OVERLAYFS

// // #define WRITE_BACK_SYNC
// #define WRITE_BACK_ASYNC

#include <linux/kernel.h>
#include <linux/uuid.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/kthread.h>
#include "ovl_entry.h"
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include "stats.h"
#include "balloc.h"

void ovl_sysfs_init(struct super_block* sb);

#define	READDIR_END			(ULONG_MAX)
#define	INVALID_CPU			(-1)
#define	ANY_CPU				(65536)
#define	FREE_BATCH			(16)
#define	DEAD_ZONE_BLOCKS		(256)

#define CACHELINE_SIZE  (64)
#define CACHELINE_MASK  (~(CACHELINE_SIZE - 1))
#define CACHELINE_ALIGN(addr) (((addr)+CACHELINE_SIZE-1) & CACHELINE_MASK)

// #define PCACHE_MOUNT_FORMAT       0x000200    /* was FS formatted on mount? */

extern int ovl_support_clwb;

#define _mm_clflush(addr)\
	asm volatile("clflush %0" : "+m" (*(volatile char *)(addr)))
#define _mm_clflushopt(addr)\
	asm volatile(".byte 0x66; clflush %0" : "+m" \
		     (*(volatile char *)(addr)))
#define _mm_clwb(addr)\
	asm volatile(".byte 0x66; xsaveopt %0" : "+m" \
		     (*(volatile char *)(addr)))

static inline void PERSISTENT_BARRIER(void)
{
	asm volatile ("sfence\n" : : );
}


extern const struct file_operations ovl_dir_operations;

extern const struct file_operations ovl_file_operations;

extern const struct inode_operations ovl_dir_inode_operations;

struct inode *ovl_new_inode(struct super_block *sb, umode_t mode, dev_t rdev);

struct inode *ovl_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, dev_t dev);

struct inode *pcache_get_inode(struct super_block *sb,
				const struct inode *dir, umode_t mode, dev_t dev, unsigned long ino);

struct inode *ovl_iget(struct super_block *sb, unsigned long ino, umode_t mode);

static inline void *ovl_get_block(struct super_block *sb, u64 block)
{
	// struct ovl_super_block *ps = ovl_get_super(sb);
    void* ps = ((struct ovl_sb_info*)(sb->s_fs_info))->virt_addr;
	// BUG_ON(block < 0 || block >= OVL_SB(sb)->initsize);
	return block ? ((void *)ps + block) : NULL;
}

static inline u64
ovl_get_addr_off(struct ovl_sb_info *sbi, void *addr)
{
	// ovl_ASSERT((addr >= sbi->virt_addr) &&
	// 		(addr < (sbi->virt_addr + sbi->initsize)));
	BUG_ON((addr < sbi->virt_addr) &&
			(addr >= (sbi->virt_addr + sbi->initsize)));
	return (u64)(addr - sbi->virt_addr);
}

static inline u64
ovl_get_block_off(struct super_block *sb, unsigned long blocknr,
		    unsigned short btype)
{
	// BUG_ON(blocknr >= OVL_SB(sb)->num_blocks);
	return (u64)blocknr << PAGE_SHIFT;
}

static inline
struct journal_ptr_pair *ovl_get_dir_journal_pointers(struct super_block *sb, int cpu)
{
	// struct ovl_sb_info *sbi = OVL_SB(sb);
	// unsigned long percpu_log_blocks = (HEAD_RESERVED_BLOCKS - 48) / sbi->cpus;
	// return (struct journal_ptr_pair *)((char *)ovl_get_block(sb,
	// 	4096 * 48) + ((unsigned long)cpu * percpu_log_blocks * 4096UL));
	struct ovl_sb_info *sbi = OVL_SB(sb);

	if (cpu >= sbi->cpus)
		BUG();

	return (struct journal_ptr_pair *)((char *)ovl_get_block(sb,
		4096 * 48) + cpu * CACHELINE_SIZE);
}

static inline
struct journal_ptr_pair *ovl_get_journal_pointers(struct super_block *sb, int cpu)
{
	// struct ovl_sb_info *sbi = OVL_SB(sb);
	// unsigned long percpu_log_blocks = (HEAD_RESERVED_BLOCKS - 48) / sbi->cpus;
	// return (struct journal_ptr_pair *)((char *)ovl_get_block(sb,
	// 	4096 * 48) + ((unsigned long)cpu * percpu_log_blocks * 4096UL));
	struct ovl_sb_info *sbi = OVL_SB(sb);

	if (cpu >= sbi->cpus)
		BUG();

	return (struct journal_ptr_pair *)((char *)ovl_get_block(sb,
		4096 * 48) + (cpu + 32) * CACHELINE_SIZE);
}


static inline void ovl_flush_buffer(void *buf, uint32_t len, bool fence)
{
	uint32_t i;

	len = len + ((unsigned long)(buf) & (CACHELINE_SIZE - 1));
	if (ovl_support_clwb) {
		for (i = 0; i < len; i += CACHELINE_SIZE)
			_mm_clwb(buf + i);
	} else {
		for (i = 0; i < len; i += CACHELINE_SIZE)
			_mm_clflush(buf + i);
	}
	/* Do a fence only if asked. We often don't need to do a fence
	 * immediately after clflush because even if we get context switched
	 * between clflush and subsequent fence, the context switch operation
	 * provides implicit fence.
	 */
	if (fence)
		PERSISTENT_BARRIER();
}

static inline void ovl_memunlock_range(struct super_block *sb, void *p,
					 unsigned long len, unsigned long *flags)
{
	
}

static inline void ovl_memlock_range(struct super_block *sb, void *p,
				       unsigned long len, unsigned long *flags)
{
}


static inline void memset_nt(void *dest, uint32_t dword, size_t length)
{
	uint64_t dummy1, dummy2;
	uint64_t qword = ((uint64_t)dword << 32) | dword;

	asm volatile ("movl %%edx,%%ecx\n"
		"andl $63,%%edx\n"
		"shrl $6,%%ecx\n"
		"jz 9f\n"
		"1:	 movnti %%rax,(%%rdi)\n"
		"2:	 movnti %%rax,1*8(%%rdi)\n"
		"3:	 movnti %%rax,2*8(%%rdi)\n"
		"4:	 movnti %%rax,3*8(%%rdi)\n"
		"5:	 movnti %%rax,4*8(%%rdi)\n"
		"8:	 movnti %%rax,5*8(%%rdi)\n"
		"7:	 movnti %%rax,6*8(%%rdi)\n"
		"8:	 movnti %%rax,7*8(%%rdi)\n"
		"leaq 64(%%rdi),%%rdi\n"
		"decl %%ecx\n"
		"jnz 1b\n"
		"9:	movl %%edx,%%ecx\n"
		"andl $7,%%edx\n"
		"shrl $3,%%ecx\n"
		"jz 11f\n"
		"10:	 movnti %%rax,(%%rdi)\n"
		"leaq 8(%%rdi),%%rdi\n"
		"decl %%ecx\n"
		"jnz 10b\n"
		"11:	 movl %%edx,%%ecx\n"
		"shrl $2,%%ecx\n"
		"jz 12f\n"
		"movnti %%eax,(%%rdi)\n"
		"12:\n"
		: "=D"(dummy1), "=d" (dummy2)
		: "D" (dest), "a" (qword), "d" (length)
		: "memory", "rcx");
}

static inline int ovl_get_cpuid(struct super_block *sb)
{
	struct ovl_sb_info *sbi = OVL_SB(sb);

	return smp_processor_id() % sbi->cpus;
}

struct ovl_range_node_lowhigh {
	__le64 range_low;
	__le64 range_high;
};



#define	RANGENODE_PER_PAGE	254

/* A node in the RB tree representing a range of pages */
struct ovl_range_node {
	struct rb_node node;
	struct vm_area_struct *vma;
	unsigned long mmap_entry;
	union {
		/* Block, inode */
		struct {
			unsigned long range_low;
			unsigned long range_high;
		};
		/* Dir node */
		struct {
			unsigned long hash;
			void *direntry;
		};
	};
	u32	csum;		/* Protect vma, range low/high */
};

struct vma_item {
	/* Reuse header of ovl_range_node struct */
	struct rb_node node;
	struct vm_area_struct *vma;
	unsigned long mmap_entry;
};

extern struct ovl_range_node *ovl_alloc_range_node_atomic(struct super_block *sb);
extern struct ovl_range_node *ovl_alloc_range_node(struct super_block *sb);

static inline unsigned long
ovl_get_numblocks(unsigned short btype)
{
	unsigned long num_blocks;

	if (btype == ovl_BLOCK_TYPE_4K) {
		num_blocks = 1;
	} else if (btype == ovl_BLOCK_TYPE_2M) {
		num_blocks = 512;
	} else {
		//btype == ovl_BLOCK_TYPE_1G
		num_blocks = 0x40000;
	}
	return num_blocks;
}

#define clear_opt(o, opt)	(o &= ~PCACHE_MOUNT_ ## opt)
#define set_opt(o, opt)		(o |= PCACHE_MOUNT_ ## opt)
#define test_opt(sb, opt)	(OVL_SB(sb)->s_mount_opt & PCACHE_MOUNT_ ## opt)

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#endif


void ovl_error_mng(struct super_block *sb, const char *fmt, ...);


/* #define ovl_dbg(s, args...)		pr_debug(s, ## args) */
#define ovl_dbg(s, args ...)		pr_info(s, ## args)
#define ovl_dbg1(s, args ...)
#define pcache_err(sb, s, args ...)	ovl_error_mng(sb, s, ## args)
#define ovl_warn(s, args ...)		pr_warn(s, ## args)
#define ovl_info(s, args ...)		pr_info(s, ## args)

static inline int memcpy_to_pmem_nocache(void *dst, const void *src,
	unsigned int size)
{
	int ret;

	ret = __copy_from_user_inatomic_nocache(dst, src, size);

	return ret;
}

// void ovl_tidy_log(struct ovl_sb_info *sbi);

void ovl_clear_stats(struct super_block *sb);

static inline uint64_t rrdtsc(void){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}


void *ovl_get_page_addr(struct xarray *pages, pgoff_t offset);
int ovl_store_page_addr(struct xarray *pages, pgoff_t offset, void* addr);

#endif