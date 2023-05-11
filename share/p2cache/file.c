#include <linux/slab.h>
#include <linux/uio.h>
#include <linux/uaccess.h>
#include <linux/falloc.h>
#include <asm/mman.h>
#include <linux/xarray.h>
#include <linux/pagemap.h>
#include "overlayfs.h"
// #include "log-index.h"
#include <linux/sort.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/smap.h>
#include "cp.h"

#define REGISTERS_1024 "%zmm0", "%zmm1", "%zmm2", "%zmm3", "%zmm4", "%zmm5", "%zmm6", "%zmm7", "%zmm8", "%zmm9", "%zmm10", "%zmm11", "%zmm12", "%zmm13", "%zmm14", "%zmm15"

#define REGISTERS_2048 "%zmm0", "%zmm1", "%zmm2", "%zmm3", "%zmm4", "%zmm5", "%zmm6", "%zmm7", "%zmm8", "%zmm9", "%zmm10", "%zmm11", "%zmm12", "%zmm13", "%zmm14", "%zmm15", "%zmm16", "%zmm17", "%zmm18", "%zmm19", "%zmm20", "%zmm21", "%zmm22", "%zmm23", "%zmm24", "%zmm25", "%zmm26", "%zmm27", "%zmm28", "%zmm29", "%zmm30", "%zmm31"

#define READ_NT_1024_ASM \
    "vmovntdqa 0*64(%[addr]),   %%zmm0 \n" \
    "vmovntdqa 1*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 2*64(%[addr]),   %%zmm2 \n" \
    "vmovntdqa 3*64(%[addr]),   %%zmm3 \n" \
    "vmovntdqa 4*64(%[addr]),   %%zmm4 \n" \
    "vmovntdqa 5*64(%[addr]),   %%zmm5 \n" \
    "vmovntdqa 6*64(%[addr]),   %%zmm6 \n" \
    "vmovntdqa 7*64(%[addr]),   %%zmm7 \n" \
    "vmovntdqa 8*64(%[addr]),   %%zmm8 \n" \
    "vmovntdqa 9*64(%[addr]),   %%zmm9 \n" \
    "vmovntdqa 10*64(%[addr]),   %%zmm10 \n" \
    "vmovntdqa 11*64(%[addr]),   %%zmm11 \n" \
    "vmovntdqa 12*64(%[addr]),   %%zmm12 \n" \
    "vmovntdqa 13*64(%[addr]),   %%zmm13 \n" \
    "vmovntdqa 14*64(%[addr]),   %%zmm14 \n" \
    "vmovntdqa 15*64(%[addr]),   %%zmm15 \n"

#define WRITE_NT_1024_ASM \
    "vmovntdq   %%zmm0, 0*64(%[waddr]) \n" \
    "vmovntdq   %%zmm1, 1*64(%[waddr]) \n" \
    "vmovntdq   %%zmm2, 2*64(%[waddr]) \n" \
    "vmovntdq   %%zmm3, 3*64(%[waddr]) \n" \
    "vmovntdq   %%zmm4, 4*64(%[waddr]) \n" \
    "vmovntdq   %%zmm5, 5*64(%[waddr]) \n" \
    "vmovntdq   %%zmm6, 6*64(%[waddr]) \n" \
    "vmovntdq   %%zmm7, 7*64(%[waddr]) \n" \
    "vmovntdq   %%zmm8, 8*64(%[waddr]) \n" \
    "vmovntdq   %%zmm9, 9*64(%[waddr]) \n" \
    "vmovntdq  %%zmm10, 10*64(%[waddr]) \n" \
    "vmovntdq  %%zmm11, 11*64(%[waddr]) \n" \
    "vmovntdq  %%zmm12, 12*64(%[waddr]) \n" \
    "vmovntdq  %%zmm13, 13*64(%[waddr]) \n" \
    "vmovntdq  %%zmm14, 14*64(%[waddr]) \n" \
    "vmovntdq  %%zmm15, 15*64(%[waddr]) \n"

#define WRITE_NT_1024_ASM1 \
    "vmovntdq   %%zmm0, 0*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm1, 1*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm2, 2*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm3, 3*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm4, 4*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm5, 5*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm6, 6*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm7, 7*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm8, 8*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm9, 9*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm10, 10*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm11, 11*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm12, 12*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm13, 13*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm14, 14*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm15, 15*64(%[waddr1]) \n"

#define READ_NT_2048_ASM \
    "vmovntdqa 0*64(%[addr]),   %%zmm0 \n" \
    "vmovntdqa 1*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 2*64(%[addr]),   %%zmm2 \n" \
    "vmovntdqa 3*64(%[addr]),   %%zmm3 \n" \
    "vmovntdqa 4*64(%[addr]),   %%zmm4 \n" \
    "vmovntdqa 5*64(%[addr]),   %%zmm5 \n" \
    "vmovntdqa 6*64(%[addr]),   %%zmm6 \n" \
    "vmovntdqa 7*64(%[addr]),   %%zmm7 \n" \
    "vmovntdqa 8*64(%[addr]),   %%zmm8 \n" \
    "vmovntdqa 9*64(%[addr]),   %%zmm9 \n" \
    "vmovntdqa 10*64(%[addr]),   %%zmm10 \n" \
    "vmovntdqa 11*64(%[addr]),   %%zmm11 \n" \
    "vmovntdqa 12*64(%[addr]),   %%zmm12 \n" \
    "vmovntdqa 13*64(%[addr]),   %%zmm13 \n" \
    "vmovntdqa 14*64(%[addr]),   %%zmm14 \n" \
    "vmovntdqa 15*64(%[addr]),   %%zmm15 \n" \
    "vmovntdqa 16*64(%[addr]),   %%zmm16 \n" \
    "vmovntdqa 17*64(%[addr]),   %%zmm17 \n" \
    "vmovntdqa 18*64(%[addr]),   %%zmm18 \n" \
    "vmovntdqa 19*64(%[addr]),   %%zmm19 \n" \
    "vmovntdqa 20*64(%[addr]),   %%zmm20 \n" \
    "vmovntdqa 21*64(%[addr]),   %%zmm21 \n" \
    "vmovntdqa 22*64(%[addr]),   %%zmm22 \n" \
    "vmovntdqa 23*64(%[addr]),   %%zmm23 \n" \
    "vmovntdqa 24*64(%[addr]),   %%zmm24 \n" \
    "vmovntdqa 25*64(%[addr]),   %%zmm25 \n" \
    "vmovntdqa 26*64(%[addr]),   %%zmm26 \n" \
    "vmovntdqa 27*64(%[addr]),   %%zmm27 \n" \
    "vmovntdqa 28*64(%[addr]),   %%zmm28 \n" \
    "vmovntdqa 29*64(%[addr]),   %%zmm29 \n" \
    "vmovntdqa 30*64(%[addr]),   %%zmm30 \n" \
    "vmovntdqa 31*64(%[addr]),   %%zmm31 \n"

#define WRITE_NT_2048_ASM \
    "vmovntdq   %%zmm0, 0*64(%[waddr]) \n" \
    "vmovntdq   %%zmm1, 1*64(%[waddr]) \n" \
    "vmovntdq   %%zmm2, 2*64(%[waddr]) \n" \
    "vmovntdq   %%zmm3, 3*64(%[waddr]) \n" \
    "vmovntdq   %%zmm4, 4*64(%[waddr]) \n" \
    "vmovntdq   %%zmm5, 5*64(%[waddr]) \n" \
    "vmovntdq   %%zmm6, 6*64(%[waddr]) \n" \
    "vmovntdq   %%zmm7, 7*64(%[waddr]) \n" \
    "vmovntdq   %%zmm8, 8*64(%[waddr]) \n" \
    "vmovntdq   %%zmm9, 9*64(%[waddr]) \n" \
    "vmovntdq  %%zmm10, 10*64(%[waddr]) \n" \
    "vmovntdq  %%zmm11, 11*64(%[waddr]) \n" \
    "vmovntdq  %%zmm12, 12*64(%[waddr]) \n" \
    "vmovntdq  %%zmm13, 13*64(%[waddr]) \n" \
    "vmovntdq  %%zmm14, 14*64(%[waddr]) \n" \
    "vmovntdq  %%zmm15, 15*64(%[waddr]) \n" \
    "vmovntdq   %%zmm16, 16*64(%[waddr]) \n" \
    "vmovntdq   %%zmm17, 17*64(%[waddr]) \n" \
    "vmovntdq   %%zmm18, 18*64(%[waddr]) \n" \
    "vmovntdq   %%zmm19, 19*64(%[waddr]) \n" \
    "vmovntdq   %%zmm20, 20*64(%[waddr]) \n" \
    "vmovntdq   %%zmm21, 21*64(%[waddr]) \n" \
    "vmovntdq   %%zmm22, 22*64(%[waddr]) \n" \
    "vmovntdq   %%zmm23, 23*64(%[waddr]) \n" \
    "vmovntdq   %%zmm24, 24*64(%[waddr]) \n" \
    "vmovntdq   %%zmm25, 25*64(%[waddr]) \n" \
    "vmovntdq  %%zmm26, 26*64(%[waddr]) \n" \
    "vmovntdq  %%zmm27, 27*64(%[waddr]) \n" \
    "vmovntdq  %%zmm28, 28*64(%[waddr]) \n" \
    "vmovntdq  %%zmm29, 29*64(%[waddr]) \n" \
    "vmovntdq  %%zmm30, 30*64(%[waddr]) \n" \
    "vmovntdq  %%zmm31, 31*64(%[waddr]) \n"

#define WRITE_NT_2048_ASM1 \
    "vmovntdq   %%zmm0, 0*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm1, 1*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm2, 2*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm3, 3*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm4, 4*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm5, 5*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm6, 6*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm7, 7*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm8, 8*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm9, 9*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm10, 10*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm11, 11*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm12, 12*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm13, 13*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm14, 14*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm15, 15*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm16, 16*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm17, 17*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm18, 18*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm19, 19*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm20, 20*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm21, 21*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm22, 22*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm23, 23*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm24, 24*64(%[waddr1]) \n" \
    "vmovntdq   %%zmm25, 25*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm26, 26*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm27, 27*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm28, 28*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm29, 29*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm30, 30*64(%[waddr1]) \n" \
    "vmovntdq  %%zmm31, 31*64(%[waddr1]) \n"


static inline unsigned long avx_copy(void* dst1, void* dst2, void* src, size_t sz)
{
    unsigned long iters = sz / 2048;
    unsigned long i;
    void *dram_src, *dram_dst, *pmem_dst;

    if(!dst2)
        for(i = 0; i < iters; i++) {
            dram_src = src + (i << 11);
            pmem_dst = dst1 + (i << 11);
            asm volatile(
                READ_NT_2048_ASM
                WRITE_NT_2048_ASM
                : [addr] "+r" (dram_src), [waddr] "+r" (pmem_dst)
                : 
                : REGISTERS_2048
            );
        }
    else
        for(i = 0; i < iters; i++) {
            dram_src = src + (i << 11);
            pmem_dst = dst1 + (i << 11);
            dram_dst = dst2 + (i << 11);
            asm volatile(
                READ_NT_2048_ASM
                WRITE_NT_2048_ASM
                WRITE_NT_2048_ASM1
                : [addr] "+r" (dram_src), [waddr] "+r" (pmem_dst), [waddr1] "+r" (dram_dst)
                : 
                : REGISTERS_2048
            );
        }

    return 0;

}

/*
"movnti %%r8, 0*8(%[waddr1]) \n"
			"movnti %%r9, 1*8(%[waddr1]) \n"
			"movnti %%r10, 2*8(%[waddr1]) \n"
			"movnti %%r11, 3*8(%[waddr1]) \n"
"movnti %%r8, 4*8(%[waddr1]) \n"
			"movnti %%r9, 5*8(%[waddr1]) \n"
			"movnti %%r10, 6*8(%[waddr1]) \n"
			"movnti %%r11, 7*8(%[waddr1]) \n"
*/

static inline unsigned long regular_copy(void* dst1, void* dst2, void* src, size_t sz)
{
	unsigned long iters = sz / 64;
    unsigned long i;
    void *dram_src, *dram_dst, *pmem_dst;
	for(i = 0; i < iters; i++) {
		dram_src = src + (i << 6);
		pmem_dst = dst1 + (i << 6);
		dram_dst = dst2 + (i << 6);
		asm volatile(
			"movq 0*8(%[addr]),	%%r8 \n"
			"movq 1*8(%[addr]),	%%r9 \n"
			"movq 2*8(%[addr]),	%%r10 \n"
			"movq 3*8(%[addr]),	%%r11 \n"
			"movnti %%r8, 0*8(%[waddr]) \n"
			"movnti %%r9, 1*8(%[waddr]) \n"
			"movnti %%r10, 2*8(%[waddr]) \n"
			"movnti %%r11, 3*8(%[waddr]) \n"
			"movnti %%r8, 0*8(%[waddr1]) \n"
			"movnti %%r9, 1*8(%[waddr1]) \n"
			"movnti %%r10, 2*8(%[waddr1]) \n"
			"movnti %%r11, 3*8(%[waddr1]) \n"
			"movq 4*8(%[addr]),	%%r8 \n"
			"movq 5*8(%[addr]),	%%r9 \n"
			"movq 6*8(%[addr]),	%%r10 \n"
			"movq 7*8(%[addr]),	%%r11 \n"
			"movnti %%r8, 4*8(%[waddr]) \n"
			"movnti %%r9, 5*8(%[waddr]) \n"
			"movnti %%r10, 6*8(%[waddr]) \n"
			"movnti %%r11, 7*8(%[waddr]) \n"
			"movnti %%r8, 4*8(%[waddr1]) \n"
			"movnti %%r9, 5*8(%[waddr1]) \n"
			"movnti %%r10, 6*8(%[waddr1]) \n"
			"movnti %%r11, 7*8(%[waddr1]) \n"
			: [addr] "+r" (dram_src), [waddr] "+r" (pmem_dst), [waddr1] "+r" (dram_dst)
			: 
			: "%r8", "%r9", "%r10", "%r11"
		);
	}
	return 0;
}


#define size_t unsigned long

#define NO_PAGE 0xFFFF000000000000

unsigned long previous_blocknr;

struct address_space dram_cache;

void *ovl_get_page_addr(struct xarray *pages, pgoff_t offset)
{
	// return NULL;
	// XA_STATE(xas, pages, offset);
	// return (void*)((unsigned long)xas_load(&xas) | NO_PAGE);
	
	return (void*)((unsigned long)xa_load(pages, offset) | NO_PAGE);
}

int ovl_store_page_addr(struct xarray *pages, pgoff_t offset, void* addr) {
	// pr_info("%s: offset: %lu, addr: %lx\n", __func__, offset, addr);
	__xa_store(pages, offset, addr, GFP_KERNEL);
	return 0;
}

// void ovl_get_page(struct address_space *mapping, pgoff_t offset)
// {
// 	// cmpxchg
// 	// pr_info("%s, %lu\n", __func__, offset);
// 	void *old_entry, *new_entry;
// 	unsigned long value;
// 	do {
// 		old_entry = xa_load(&mapping->i_pages, offset);
// 		// value = xa_to_value(old_entry);
// 		// new_entry = xa_mk_value(value + 1);
// 		new_entry = (unsigned long)old_entry - 0x0001000000000000; 
// 	} while(xa_cmpxchg(&mapping->i_pages, offset, old_entry, new_entry, GFP_KERNEL) != old_entry);
// }

// void ovl_put_page(struct address_space *mapping, pgoff_t offset)
// {
// 	// cmpxchg
// 	// pr_info("%s, %lu\n", __func__, offset);
// 	void *old_entry, *new_entry;
// 	unsigned long value;
// 	do {
// 		old_entry = xa_load(&mapping->i_pages, offset);
// 		// value = xa_to_value(old_entry);
// 		// new_entry = xa_mk_value(value - 1);
// 		new_entry = (unsigned long)old_entry + 0x0001000000000000;
// 	} while(xa_cmpxchg(&mapping->i_pages, offset, old_entry, new_entry, GFP_KERNEL) != old_entry);
// }

int ovl_page_readable(struct address_space *mapping, pgoff_t offset)
{
	// return true;
	// return false;
	// pr_info("query page %lu, %lx\n", offset, (unsigned long)xa_load(&mapping->i_pages, offset) & NO_PAGE);
	return ((unsigned long)xa_load(&mapping->i_pages, offset) & NO_PAGE) == NO_PAGE;
}



void wait_on_page(struct address_space *mapping, pgoff_t offset)
{
	// pr_info("offset: %lu, entry: %lx\n", offset, xa_load(&mapping->i_pages, offset));
	while(!ovl_page_readable(mapping, offset));
}

static int ovl_append_file_write_entry(struct super_block *sb, struct ovl_inode_info_header *sih, unsigned long ino,
										int write_type, loff_t pos, unsigned long blocknr, void *buf, size_t len)
{
	// struct journal_ptr_pair *pair;
	size_t size = sizeof(struct ovl_file_write_entry), ret;
	struct ovl_file_write_entry *entry;

	// pair = ovl_get_journal_pointers(sb, 0);
	
	entry = ovl_get_block(sb, sih->log_tail);

	entry->entry_type = 0;
	entry->write_type = write_type;
	entry->mtime = rrdtsc();
	entry->ino = ino;
	entry->pos = pos;
	entry->blocknr = blocknr;
	entry->len = len;

	// pr_info("append log tail: %lu, write_type: %d\n", sih->log_tail, write_type);

	sih->log_tail += (size + 63) & ~63;

	if(write_type == 4) {
		// pr_info("copy data to %lu\n", ovl_get_block(sb, sih->log_tail));
		// ret = copy_from_user(ovl_get_block(sb, sih->log_tail), buf, len);
		ret = __cp(ovl_get_block(sb, sih->log_tail), buf, len, 0);
		BUG_ON(ret);
		sih->log_tail += (len + 63) & ~63;
	}

	// __iget
	

	return 0;
}

static int ovl_persist_log(struct super_block *sb, struct ovl_inode_info_header *sih)
{
	struct journal_ptr_pair *pair;
	int cpuid = smp_processor_id();

	PERSISTENT_BARRIER();

	pair = ovl_get_journal_pointers(sb, cpuid);

	pair->journal_tail = sih->log_tail;

	// pr_info("%s: cpuid %d, %lx\n", __func__, cpuid, pair->journal_tail);

	// ovl_flush_buffer(pair, CACHELINE_SIZE, 1);
	PERSISTENT_BARRIER();
	return 0;
}

// static void wakeup_bg(struct ovl_sb_info *sbi)
// {
// 	atomic_set(&sbi->bg_signal, 1);
// 	if (!waitqueue_active(&sbi->bg_wait))
// 		return;


// 	// nova_dbg("Wakeup snapshot cleaner thread\n");
// 	wake_up_interruptible(&sbi->bg_wait);
// }

// static struct page *ovl_pagecache_get_page(struct address_space *mapping, pgoff_t offset)
// {
// 	struct page *page;


// 	page = find_get_entry(mapping, offset);
// 	if (xa_is_value(page))
// 		page = NULL;

// 	if (!page) {
// 		int err;
		
// 		page = __page_cache_alloc(GFP_HIGHUSER);
// 		if (!page)
// 			return NULL;

// 		add_to_page_cache_locked(page, &dram_cache, offset, GFP_HIGHUSER);
// 		/*
// 		 * add_to_page_cache_lru locks the page, and for mmap we expect
// 		 * an unlocked page.
// 		 */
// 		// if (page && (fgp_flags & FGP_FOR_MMAP))
// 		// 	unlock_page(page);
// 	}

// 	return page;
// }


/*
 * Generally, we can split a userspace buffer into 3 segments
 * A head segment, a body segment(multiple aligned blocks), a tail segment
 */

int split_buffer(size_t pos, size_t len, size_t boundary[3][2])
{
	int i;
	
	for(i = 0; i < 3 && len; i++) {
		// the start pos
		boundary[i][0] = pos;
		if (!(pos & ~PAGE_MASK)) { // start at an aligned pos
			// because this segment starts at an aligned pos,
			// it can have two cases
			boundary[i][1] = len < PAGE_SIZE ?
								len : // tail
								len & PAGE_MASK; // multiple aligend blocks
		} else { // start at an unaligned pos
			boundary[i][1] = (pos & ~PAGE_MASK) + len <= PAGE_SIZE ? len : (PAGE_SIZE - (pos & ~PAGE_MASK));
		}
		len -= boundary[i][1];
		pos += boundary[i][1];
	}
	return 0;
}

static ssize_t do_ovl_cow_file_write0(struct file *filp,
	const char __user *buf,	size_t len, loff_t *ppos)
{
	struct address_space *mapping = filp->f_mapping;
	struct inode	*inode = mapping->host;
	struct super_block *sb = inode->i_sb;
	struct ovl_inode_info_header *sih = OVL_IH(inode);
	struct ovl_sb_info *sbi = sb->s_fs_info;

	unsigned long blocksize_mask = sb->s_blocksize - 1;
	unsigned long num_blocks, num_blocks_need;

	loff_t pos = *ppos, curr_pos = *ppos;
	
	void *kmem, *kmem_dram;
	int allocated;
	unsigned long blocknr, i;
	int cpuid = smp_processor_id();
	
	loff_t s, offset;
	size_t l, ret;

	void *addr;

	size_t written = 0;
	char __user *curr_buf = buf;

	struct page *dram_page;

	size_t boundary[3][2] = {{0,0},{0,0},{0,0}};

	unsigned long write_back[3][3] = {{0,0,0},{0,0,0},{0,0,0}};

	// struct xa_state *xas;

	if (filp->f_flags & O_APPEND)
		pos = i_size_read(inode);

	split_buffer(pos, len, boundary);

	// if(boundary[0][0] % 256 == 0)
	// 	pr_info("%s, {%lu, %lu}, {%lu, %lu}, {%lu, %lu}\n", filp->f_path.dentry->d_name.name, boundary[0][0], boundary[0][1], boundary[1][0], boundary[1][1], boundary[2][0], boundary[2][1]);

	// ASM_STAC
	
	sih->log_tail = sih->log_head = ovl_get_journal_pointers(sb, cpuid)->journal_tail;

	for(i = 0; i < 3; i++) {
		
		s = boundary[i][0]; // start pos
		l = boundary[i][1]; // len

		offset = ~PAGE_MASK & s;

		// BUG_ON(s && !l);
		// BUG_ON(offset && offset + l > PAGE_SIZE);
		// BUG_ON(!offset && (l > PAGE_SIZE) && (~PAGE_MASK & l));

		// pr_info("start: %lu, len: %lu\n", s, l);

		if(!s && !l)
			break;
		
		// ideal case, multiple aligned blocks
		if(!(s & ~PAGE_MASK) && (l & PAGE_MASK)) {
			unsigned long m = 0;
			// pr_info("aligned start: %lu, len: %lu\n", s, l);
			// BUG_ON(s >= 8589934592UL);
			num_blocks_need = l >> PAGE_SHIFT;
			allocated = ovl_new_data_blocks(sb, NULL, &blocknr, 0,
				 num_blocks_need, ALLOC_NO_INIT, ANY_CPU,
				 ALLOC_FROM_HEAD);

			BUG_ON(allocated != num_blocks_need);

			// if()

			kmem = ovl_get_block(inode->i_sb,
					ovl_get_block_off(sb, blocknr, 0));

			
			// BUG_ON((curr_buf - buf) + l > len);


			// ret = memcpy_to_pmem_nocache(kmem, curr_buf, l);
			// BUG_ON(ret);

			// while()

			

			// if(sbi->enable_dram_page) {
			// 	kmem_dram = ovl_get_page_addr(&sih->dram_pages, (s >> PAGE_SHIFT));
			// 	if(kmem_dram != NO_PAGE) {
			// 		ret = copy_from_user(kmem_dram, curr_buf, l);
			// 		BUG_ON(ret);
			// 	} else {
			// 		kmem_dram = alloc_pages(GFP_NOWAIT, 0);
			// 		BUG_ON(!kmem_dram);
			// 		copy_from_user(kmem_dram, curr_buf, l);

			// 	}
			// }
			

			// pr_info("%lu\n", kmem);

			// while (allocated--)
			// 	ovl_store_page_addr(&sih->pmem_pages, (s >> PAGE_SHIFT) + allocated, kmem + (allocated << PAGE_SHIFT)); 

			// while(m < allocated) {
			// 	// if(s % 1048576 == 0)
			// 	// 	pr_info("s: %lu, m: %lu\n", s, m);
				
			// 	if(sbi->enable_dram_page) {
			// 		kmem_dram = ovl_get_page_addr(&sih->dram_pages, (s >> PAGE_SHIFT) + m);
			// 		if(kmem_dram != NO_PAGE) {
			// 			ret = copy_from_user(kmem_dram, curr_buf + (m << PAGE_SHIFT), PAGE_SIZE);
			// 			BUG_ON(ret);
			// 		} else {
			// 			// kmem_dram = alloc_pages(GFP_NOWAIT | GFP_NOIO, 0);
			// 			// kmem_dram = vmalloc(4096);
			// 			kmem_dram = sbi->dram_pool + (sbi->dram_alloc[cpuid] << 12);
			// 			sbi->dram_alloc[cpuid]++;
			// 			BUG_ON(!kmem_dram);
			// 			ret = __cp(kmem_dram, curr_buf + (m << PAGE_SHIFT) , PAGE_SIZE, 0);
			// 			ret = __cp(kmem + (m << PAGE_SHIFT), curr_buf + (m << PAGE_SHIFT), PAGE_SIZE, 0);
			// 			BUG_ON(ret);
			// 			// ovl_store_page_addr(&sih->dram_pages, (s >> PAGE_SHIFT) + m, kmem_dram);
			// 			// ovl_store_page_addr(&sih->pmem_pages, (s >> PAGE_SHIFT) + m, kmem + (m << PAGE_SHIFT));
			// 		}
			// 	} else {
			// 	// ret = copy_from_user(kmem + (m << PAGE_SHIFT), curr_buf + (m << PAGE_SHIFT), PAGE_SIZE);
			// 		ret = __cp(kmem + (m << PAGE_SHIFT), curr_buf + (m << PAGE_SHIFT), PAGE_SIZE, 0);
			// 		BUG_ON(ret);
				
			// 		ovl_store_page_addr(&sih->pmem_pages, (s >> PAGE_SHIFT) + m, kmem + (m << PAGE_SHIFT));
			// 	}
			// 	m++;
			// }
			kmem_dram = sbi->dram_pool + (sbi->dram_alloc[cpuid] << 12);
			sbi->dram_alloc[cpuid] += allocated;
			// ret = memcpy_to_pmem_nocache(kmem, curr_buf, l);
			// ret = __cp(kmem_dram, curr_buf, l, 0);
			// ret = __cp(kmem, curr_buf, l, 0);
			BUG_ON(sbi->dram_alloc[cpuid] >= (128849018880UL / PAGE_SIZE / sbi->cpus) * (cpuid+1));
			if(l >= 16384) {
				stac();
				avx_copy(kmem, kmem_dram, curr_buf, l);
				clac();
			} else {
				ret = __cp(kmem, curr_buf, l, 0);
				ret = __cp(kmem_dram, curr_buf, l, 0);
				BUG_ON(ret);
				
			}
			while(m < allocated) {
				ovl_store_page_addr(&sih->dram_pages, (s >> PAGE_SHIFT) + m, kmem_dram + (m << PAGE_SHIFT));
				ovl_store_page_addr(&sih->pmem_pages, (s >> PAGE_SHIFT) + m, kmem + (m << PAGE_SHIFT));
				m++;
			}
			// BUG_ON(ret);

			// ret = memcpy_to_pmem_nocache(kmem + (m << PAGE_SHIFT), curr_buf + (m << PAGE_SHIFT), l);
			// BUG_ON(ret);
			
			if(!sbi->write_back_sync)
				xa_store_range(&sih->log_index, s, (s + l - 1), NULL, GFP_KERNEL);


			
			ovl_append_file_write_entry(sb, sih, inode->i_ino, 1, s, blocknr, NULL, l);

			curr_buf += l;
			written += l;
			
		} else {
			// pr_info("unaligned, start: %lu, len: %lu\n", s, l);
			if (s >= i_size_read(inode)) { // append
				
				kmem = ovl_get_page_addr(&sih->pmem_pages, s >> PAGE_SHIFT);

				if(kmem != NO_PAGE) { // page exists, append right after the end of the file
					// pr_info("append 1, start: %lu, len: %lu\n", s, l);
					// ret = copy_from_user(kmem + offset, curr_buf, l);
					ret = __cp(kmem + offset, curr_buf, l, 0);
					BUG_ON(ret);
					
					if(sbi->enable_dram_page) {
						kmem_dram = ovl_get_page_addr(&sih->dram_pages, s >> PAGE_SHIFT);
						if(kmem_dram != NO_PAGE) {
							ret = copy_from_user(kmem_dram + offset, curr_buf, l);
							BUG_ON(ret);
						} else {
							BUG();
						}
					}


					ovl_append_file_write_entry(sb, sih, inode->i_ino, 2, s, 0, NULL, l);

					curr_buf += l;
					written += l;
				} else { // append, "aligned start" or "hole"
					// pr_info("append 2, start: %lu, len: %lu\n", s, l);
					num_blocks_need = 1;
					allocated = ovl_new_data_blocks(sb, NULL, &blocknr, 0,
									num_blocks_need, ALLOC_NO_INIT, ANY_CPU,
									ALLOC_FROM_HEAD);

					BUG_ON(allocated != num_blocks_need);

					kmem = ovl_get_block(inode->i_sb,
						ovl_get_block_off(sb, blocknr, 0));

					// BUG_ON((curr_buf - buf) + l > len);
					// ret = copy_from_user(kmem + offset, curr_buf, l);
					ret = __cp(kmem + offset, curr_buf, l, 0);
					BUG_ON(ret);

					if(sbi->enable_dram_page) {
						kmem_dram = ovl_get_page_addr(&sih->dram_pages, s >> PAGE_SHIFT);
						if(kmem_dram != NO_PAGE) {
							ret = copy_from_user(kmem_dram + offset, curr_buf, l);
							BUG_ON(ret);
						} else {
							// kmem_dram = get_zeroed_page(GFP_NOWAIT);
							kmem_dram = sbi->dram_pool + (sbi->dram_alloc[cpuid] << 12);
							sbi->dram_alloc[cpuid]++;
							BUG_ON(!kmem_dram);
							ret = copy_from_user(kmem_dram + offset, curr_buf, l);
							BUG_ON(ret);
							ovl_store_page_addr(&sih->dram_pages, s >> PAGE_SHIFT, kmem_dram);
						}
					}

					// ret = memcpy_to_pmem_nocache(kmem, sbi->zeroed_page, offset);
					// BUG_ON(ret);

					// ret = memcpy_to_pmem_nocache(kmem + offset + l, sbi->zeroed_page, (PAGE_SIZE - offset - l));
					// BUG_ON(ret);

					ovl_store_page_addr(&sih->pmem_pages, s >> PAGE_SHIFT, kmem);

					ovl_append_file_write_entry(sb, sih, inode->i_ino, 3, s, blocknr, NULL, l);

					curr_buf += l;
					written += l;
				}

				// if(offset) { // do not need new page
				// 	// pr_info("type2");
				// 	kmem = ovl_get_page_addr(&sih->pmem_pages, s >> PAGE_SHIFT);
				// 	BUG_ON(!kmem);

				// 	BUG_ON((curr_buf - buf) + l > len);
				// 	BUG_ON((unsigned long)curr_buf < (unsigned long)buf);
				// 	ret = memcpy_to_pmem_nocache(kmem + offset, curr_buf, l);
				// 	// BUG_ON(ret);
				// 	if(ret) {
				// 		pr_info("%lx, %lu, %lx, %lu, %lu, %lu\n", kmem, offset, curr_buf, l, s, i_size_read(inode));
				// 		BUG();
				// 	}
					

				// 	ovl_append_file_write_entry(sb, sih, inode->i_ino, 2, s, 0, NULL, l);

				// 	curr_buf += l;
				// 	written += l;

					
				// } else { // need new page
				// 	// pr_info("type3");
				// 	num_blocks_need = 1;
				// 	allocated = ovl_new_data_blocks(sb, NULL, &blocknr, 0,
				// 					num_blocks_need, ALLOC_NO_INIT, ANY_CPU,
				// 					ALLOC_FROM_HEAD);

				// 	BUG_ON(allocated != num_blocks_need);

				// 	kmem = ovl_get_block(inode->i_sb,
				// 		ovl_get_block_off(sb, blocknr, 0));

				// 	BUG_ON((curr_buf - buf) + l > len);
				// 	ret = memcpy_to_pmem_nocache(kmem, curr_buf, l);
				// 	BUG_ON(ret);

				// 	ovl_store_page_addr(&sih->pmem_pages, s >> PAGE_SHIFT, kmem);

				// 	// xa_store_range(&sih->log_index, s, (s + l - 1), NULL, GFP_KERNEL);

				// 	ovl_append_file_write_entry(sb, sih, inode->i_ino, 3, s, blocknr, NULL, l);

				// 	curr_buf += l;
				// 	written += l;
				// }



			} else { // overwrite

				// ovl_append_file_write_entry(sb, sih, inode->i_ino, 4, s, 0, NULL, l);
				// pr_info("pos: %lu, len: %lu\n", pos, len);
				// for(;i>=0;i--)
				// 	pr_info("i: %lu, pos: %lu, len: %lu\n", i, s, l);


				// BUG();

				// index_node = kzalloc(sizeof(struct log_index_node),GFP_KERNEL);

				// index_node->key = (s << PAGE_SHIFT) | l;
				// index_node->ptr = sih->log_tail;

				kmem = ovl_get_page_addr(&sih->pmem_pages, s >> PAGE_SHIFT);
				if(kmem != NO_PAGE) {
					// pr_info("overwrite 1, start: %lu, len: %lu\n", s, l);
					addr = sih->log_tail;

					ovl_append_file_write_entry(sb, sih, inode->i_ino, 4, s, 0, curr_buf, l);

					if(sbi->enable_dram_page) {
						kmem_dram = ovl_get_page_addr(&sih->dram_pages, s >> PAGE_SHIFT);
						if(kmem_dram != NO_PAGE) {
							ret = copy_from_user(kmem_dram + offset, curr_buf, l);
							BUG_ON(ret);
						} else {
							// kmem_dram = get_zeroed_page(GFP_NOWAIT);
							// BUG_ON(!kmem_dram);
							// ret = copy_from_user(kmem_dram + offset, curr_buf, l);
							// ovl_store_page_addr(&sih->dram_pages, s >> PAGE_SHIFT, kmem_dram);
							// kmem_dram = alloc_pages(GFP_NOWAIT, 0);
							kmem_dram = sbi->dram_pool + (sbi->dram_alloc[cpuid] << 12);
							sbi->dram_alloc[cpuid]++;
							BUG_ON(!kmem_dram);
							copy_page(kmem_dram, kmem);
							ret = copy_from_user(kmem_dram + offset, curr_buf, l);
							BUG_ON(ret);
							ovl_store_page_addr(&sih->dram_pages, s >> PAGE_SHIFT, kmem_dram);
						}
					}

					if(sbi->write_back_sync) {
					
						write_back[i][0] = s;
						write_back[i][1] = l;
						write_back[i][2] = curr_buf;
					} else {
						// log_index_insert_node(&sih->log_index, index_node);
						xa_store(&sih->log_index, (s << PAGE_SHIFT) | l, addr, GFP_KERNEL);
						// (1024 << 12) | 1024
						// (1024, 2048), (1025, 2047)
						// 0 << (12 << 1)
					}

				} else { // overwriting a hole
					// pr_info("overwrite 2, start: %lu, len: %lu\n", s, l);
					num_blocks_need = 1;
					allocated = ovl_new_data_blocks(sb, NULL, &blocknr, 0,
									num_blocks_need, ALLOC_INIT_ZERO, ANY_CPU,
									ALLOC_FROM_HEAD);

					BUG_ON(allocated != num_blocks_need);

					kmem = ovl_get_block(inode->i_sb,
						ovl_get_block_off(sb, blocknr, 0));


					BUG_ON((curr_buf - buf) + l > len);
					// ret = copy_from_user(kmem + offset, curr_buf, l);
					ret = __cp(kmem + offset, curr_buf, l, 0);
					BUG_ON(ret);

					// ret = memcpy_to_pmem_nocache(kmem, sbi->zeroed_page, offset);
					// BUG_ON(ret);

					// ret = memcpy_to_pmem_nocache(kmem + offset + l, sbi->zeroed_page, (PAGE_SIZE - offset - l));
					// BUG_ON(ret);

					ovl_store_page_addr(&sih->pmem_pages, s >> PAGE_SHIFT, kmem);
					

					if(sbi->enable_dram_page) {
						kmem_dram = ovl_get_page_addr(&sih->dram_pages, s >> PAGE_SHIFT);
						if(kmem_dram != NO_PAGE) {
							BUG();
							// ret = copy_from_user(kmem_dram + offset, curr_buf, l);
							// BUG_ON(ret);
						} else {
							// kmem_dram = get_zeroed_page(GFP_NOWAIT);
							kmem_dram = sbi->dram_pool + (sbi->dram_alloc[cpuid] << 12);
							sbi->dram_alloc[cpuid]++;
							BUG_ON(!kmem_dram);
							ret = copy_from_user(kmem_dram + offset, curr_buf, l);
							BUG_ON(ret);
							ovl_store_page_addr(&sih->dram_pages, s >> PAGE_SHIFT, kmem_dram);
						}
					}

					addr = sih->log_tail;
					ovl_append_file_write_entry(sb, sih, inode->i_ino, 5, s, blocknr, NULL, l);
					// xa_store(&sih->partial_writes, s >> PAGE_SHIFT, addr, GFP_KERNEL);

				}

				

				curr_buf += l;
				written += l;
			}
		}
		
	}

	BUG_ON(written != len);


	ovl_persist_log(sb, sih);

	// if(sbi->write_back_sync) {

	// 	for(i = 0; i < 3; i++) {
	// 		if(write_back[i][0] && write_back[i][1]) {
	// 			// pr_info("overwrite\n");
	// 			kmem = ovl_get_page_addr(&sih->pmem_pages, write_back[i][0] >> PAGE_SHIFT);
	// 			BUG_ON(!kmem);
	// 			BUG_ON(write_back[i][1] + (write_back[i][2] - (unsigned long)buf) > len);
	// 			__cp(kmem + (write_back[i][0] & ~PAGE_MASK), (void*)write_back[i][2], write_back[i][1], 0);
	// 		}
	// 	}

	// }



	pos += written;
	
	*ppos = pos;

	if(pos > i_size_read(inode)){
		i_size_write(inode, pos);
	}

	// ASM_CLAC

	return written;

}

static ssize_t do_ovl_dax_file_write(struct file *filp, const char __user *buf,
				   size_t len, loff_t *ppos)
{
	// struct address_space *mapping = filp->f_mapping;
	// struct inode *inode = mapping->host;

	return do_ovl_cow_file_write0(filp, buf, len, ppos);

	// if (test_opt(inode->i_sb, DATA_COW))
	// 	return do_ovl_cow_file_write(filp, buf, len, ppos);
	// else
	// 	return do_ovl_inplace_file_write(filp, buf, len, ppos);
}

static int compare(const void *lhs, const void *rhs) {
	struct ovl_file_write_entry *lentry = *(struct ovl_file_write_entry **)lhs;
	struct ovl_file_write_entry *rentry = *(struct ovl_file_write_entry **)rhs;

	// pr_info("%lu, %lu", lentry, rentry);

	// if(lentry->pos < rentry->pos)
	// 	return -1;
	// else if(lentry->pos > rentry->pos)
	// 	return 1;

	// return 0;
	
	if(lentry->mtime > rentry->mtime)
		return -1;
	else if(lentry->mtime < rentry->mtime)
		return 1;
	else {
		BUG();
		return 0;
	}
}

static ssize_t
do_dax_mapping_read(struct file *filp, char __user *buf,
	size_t len, loff_t *ppos)
{
	struct inode *inode = filp->f_mapping->host;
	struct super_block *sb = inode->i_sb;
	struct ovl_inode_info *si = OVL_I(inode);
	struct ovl_inode_info_header *sih = &si->header;
	struct ovl_file_write_entry *e;
	// struct ovl_file_write_entry *entry, *e;
	// struct ovl_file_write_entry *entryc, entry_copy;
	// pgoff_t index, end_index;
	pgoff_t index;
	unsigned long offset;
	loff_t isize, pos;
	size_t copied = 0, error = 0, ret;
	int cpuid = smp_processor_id();
	// unsigned long blocknr, blocknr_start;

	struct ovl_sb_info *sbi = sb->s_fs_info;

	struct dlist_node* tmp_list = sbi->dlist[cpuid];

	unsigned long *tmp_addr = sbi->tmp_addr[cpuid];

	loff_t bio_pos;

	

	// struct ovl_file_write_entry 

	// pr_info("%s\n", __func__);

	pos = *ppos;
	index = pos >> PAGE_SHIFT;
	offset = pos & ~PAGE_MASK;

	// pr_info("%s, %lu, %lu, %lu, %lu, a1\n", filp->f_path.dentry->d_name.name, pos, index, offset, len);

	if (!access_ok(buf, len)) {
		error = -EFAULT;
		goto out;
	}
	// pr_info("buf: %lu\n", buf);
	// pr_info("%s, a2\n", filp->f_path.dentry->d_name.name);


	isize = i_size_read(inode);
	if (!isize)
		goto out;

	// pr_info("%s, a2\n", filp->f_path.dentry->d_name.name);


	if (len > isize - pos)
		len = isize - pos;

	if (len <= 0)
		goto out;

	// wakeup_bg(OVL_SB(sb));


	do {
		// pr_info("read loop\n");
		unsigned long nr;
		// unsigned long nvmm;
		void *dax_mem = NULL;
		void *dax_pmem = NULL;
		// int zero = 0;
		int i, j, k;

		// char map[512];
		unsigned long curr_offset, idx;

		unsigned pstart, pend;


		struct dlist_node *dlist_head, *curr_node, *new_node, *next_node;



// memcpy:
		nr = 4096 - offset;
		if(nr > len - copied)
			nr = len - copied;

		// dax_mem = ovl_get_block(inode->i_sb,
		// 				ovl_get_block_off(sb, blocknr_start + (index * 8 + (cpuid % 8)), 0));

		// while(!ovl_page_readable(inode->i_mapping, index));
		// wait_on_page(inode->i_mapping, index);


		// if(ovl_get_page_addr())

		dax_mem = ovl_get_page_addr(&sih->dram_pages, index);
		if(dax_mem == NO_PAGE) {
			dax_mem = ovl_get_page_addr(&sih->pmem_pages, index);
		}
		
		// dax_mem = ovl_get_page_addr(&sih->pmem_pages, index);
		if(dax_mem == NO_PAGE) { // hole
			dax_mem = sbi->zeroed_page;
		}
		
		// e = xa_load(&sih->partial_writes, index);
		e = NULL;
		if(e) {
			
			// pr_info("calling underlying read\n");
			
			// ret = vfs_read(sbi->underlying_file, (void __user *)(buf + copied), nr, offset);
			// bio_pos = 0;
			// ret = kernel_read(sbi->underlying_file, sbi->bio_buffer, nr, &bio_pos);
			// __copy_to_user(buf + copied, sbi->bio_buffer, nr);
			// // BUG_ON(ret != nr);
			// copied += nr;

		} else if(!sbi->write_back_sync) {
			i = 0; 

			xa_for_each_start(&sih->log_index, idx, e, index << (PAGE_SHIFT << 1)) {

			

				if(!e)
					BUG();
				
				if(idx >> (PAGE_SHIFT << 1) > index)
					break;

				e = (struct ovl_file_write_entry*) ovl_get_block(inode->i_sb, e);

				// pr_info("%lu, %lu, %lu, %lu", e->pos, e->len, offset, ((e->pos + e->len) & ~PAGE_MASK));

				if((e->pos + e->len) > ((index << PAGE_SHIFT) + offset))
					tmp_addr[i++] = (unsigned long)e;
					// continue;

				// if(offset + nr <= e->pos & ~PAGE_MASK)
				// 	continue;

				

				// if(e->len) {
					
				// 	__copy_to_user(buf + copied + (e->pos & ~PAGE_MASK), )
				// }
				
			}

			sort(tmp_addr, i, sizeof(unsigned long), &compare, NULL);



			dlist_head = NULL;
			curr_node = NULL;
			next_node = NULL;
			k = 0;
			curr_offset = offset;

			for(j = 0; j < i; j++) {
				e = (struct ovl_file_write_entry *)tmp_addr[j];
				pstart = (unsigned int)(e->pos & ~PAGE_MASK);
				pend = pstart + (unsigned int)e->len;

				
				if(dlist_head) {
					curr_node = dlist_head;
					if(pstart < curr_node->start) { // insert before the head
						new_node = &tmp_list[k++];
						new_node->start = offset > pstart ? offset : pstart;
						// new_node->end = pend <= curr_node->start ? pend : curr_node->start;
						new_node->end = min(pend, curr_node->start);
						new_node->ptr = e;
						new_node->next = curr_node;
						new_node->prev = NULL;
						dlist_head = new_node;
						
					} else if(pstart == curr_node->start) {
						BUG();
					}

					

					while(curr_node && pend > curr_node->end) {
						next_node = curr_node->next;
						if(next_node && curr_node->end == next_node->start) {
							curr_node = next_node;
							continue;
						}
						
						new_node = &tmp_list[k++];
						new_node->start = pstart > curr_node->end ? pstart : curr_node->end;
						// new_node->end = next_node ? (pend < next_node->start ? pend : next_node->start) : pend;
						new_node->end = next_node ? min(pend, next_node->start) : pend;
						
						new_node->ptr = e;
						new_node->next = next_node;
						new_node->prev = curr_node;
						curr_node->next = new_node;
						if(next_node)
							next_node->prev = new_node;
						
						curr_node = next_node;
						if (new_node->end > offset + nr) {
							new_node->end = offset + nr;
							break;
						}
					}
				} else {
					new_node = &tmp_list[k++];
					new_node->start = max(pstart, offset);
					new_node->end = min(pend, offset + nr);
					new_node->ptr = e;
					new_node->next = NULL;
					new_node->prev = NULL;
					dlist_head = new_node;
				}
			}


			// do the real copy
			curr_node = dlist_head;
			curr_offset = offset;
			while(curr_offset < offset + nr) {
				
				if(curr_node && curr_offset < curr_node->start) {
					// copy from page
					ret = __copy_to_user(buf + copied, dax_mem + curr_offset, curr_node->start - curr_offset);
					BUG_ON(ret);
					
					copied += curr_node->start - curr_offset;
					curr_offset = curr_node->start;
					// continue;
				} else if(curr_node && curr_offset == curr_node->start) {
					// copy from log entry
					// pr_info("%lu, %lu, %lu, %lu, %lu, %lu, %lu", curr_node, curr_node->ptr, curr_node->start, buf, copied, ((struct ovl_file_write_entry*)(curr_node->ptr))->pos, (curr_node->ptr + 64) + (curr_node->start - ((struct ovl_file_write_entry*)(curr_node->ptr))->pos & ~PAGE_MASK));
					ret = __copy_to_user(buf + copied, (curr_node->ptr + 64) + (curr_node->start - ((struct ovl_file_write_entry*)(curr_node->ptr))->pos & ~PAGE_MASK), curr_node->end - curr_node->start);
					BUG_ON(ret);
					
					copied += curr_node->end - curr_node->start;
					curr_offset = curr_node->end;

					curr_node = curr_node->next;
					
					// continue;
				} else { 
					// copy the remaining
					
					ret = __copy_to_user(buf + copied, dax_mem + curr_offset, offset + nr - curr_offset);
					// pr_info("%lu, %lu, %lu, %lu, %lu, %lu", buf, copied, dax_mem, curr_offset, ret, offset + nr);
					// BUG_ON(ret);
					if(ret) {
						pr_info("%lu, %lu, %lu, %lu, %lu, %lu", buf, copied, dax_mem, curr_offset, ret, offset + nr);
						BUG();
					}

					copied += offset + nr - curr_offset;
					curr_offset = offset + nr;
					
					// continue;
				}
			}
		
		} else {
			
			BUG_ON(copied + nr > len);
			// pr_info("%lx, %lu, %lx, %lu, %lu\n", buf, copied, dax_mem, offset, nr);

			ret = __copy_to_user(buf + copied, dax_mem + offset, nr);
			// dax_pmem = ovl_get_page_addr(&sih->pmem_pages, index);
			// __copy_to_user(buf + copied, dax_pmem, 2048);
			// __copy_to_user(buf + copied + 2048, dax_mem, 2048);
			
			BUG_ON(ret);

			copied += nr;

			// ============================

			// pr_info("calling underlying read\n");
			
			// ret = vfs_read(sbi->underlying_file, (void __user *)(buf + copied), nr, offset);
			// BUG_ON(ret != nr);

			// copied += nr;
			
		}
		
		offset += nr;
		index += offset >> PAGE_SHIFT;
		offset &= ~PAGE_MASK;	
		
	} while(copied < len);

	BUG_ON(copied > len);

	// pr_info("copied: %lu\n", copied);

out:
	*ppos = pos + copied;
	if (filp)
		file_accessed(filp);

	// pr_info("accessed");
	
	return copied;
}

// [1,4095][4096,8191]
//

static ssize_t ovl_wrap_rw_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	struct file *filp = iocb->ki_filp;
	struct inode *inode = filp->f_mapping->host;
	ssize_t ret = -EIO;
	ssize_t written = 0;
	unsigned long seg;
	unsigned long nr_segs = iter->nr_segs;
	const struct iovec *iv = iter->iov;

	unsigned long flags;

	// pr_info("rw begin\n");
	// pr_info("name: %s\n", filp->f_path.dentry->d_name.name);

	// ovl_dbgv("%s %s: %lu segs\n", __func__,
	// 		iov_iter_rw(iter) == READ ? "read" : "write",
	// 		nr_segs);

	if (iov_iter_rw(iter) == WRITE)  {
		sb_start_write(inode->i_sb);
		// local_irq_save(flags);
		// preempt_disable();
		// inode_lock(inode);
	} else {
		// pr_info("begin read");
		// inode_lock_shared(inode);
	}
		
	iv = iter->iov;
	for (seg = 0; seg < nr_segs; seg++) {
		if (iov_iter_rw(iter) == READ) {
			// pr_info("read");
			// pr_info("name: %s\n", filp->f_path.dentry->d_name.name);
			ret = do_dax_mapping_read(filp, iv->iov_base,
						  iv->iov_len, &iocb->ki_pos);
		} else if (iov_iter_rw(iter) == WRITE) {
			ret = do_ovl_dax_file_write(filp, iv->iov_base,
						     iv->iov_len, &iocb->ki_pos);
		} else {
			BUG();
		}
		if (ret < 0)
			goto err;

		if (iter->count > iv->iov_len)
			iter->count -= iv->iov_len;
		else
			iter->count = 0;

		written += ret;
		iter->nr_segs--;
		iv++;
	}
	ret = written;
err:
	if (iov_iter_rw(iter) == WRITE)  {
		// preempt_enable();
		// local_irq_restore(flags);
		// inode_unlock(inode);
		sb_end_write(inode->i_sb);
	} else {
		// inode_unlock_shared(inode);
	}

	// pr_info("rw end\n");

	// ovl_END_TIMING(wrap_iter_t, wrap_iter_time);
	return ret;
}

// static int ovl_fsync(struct file *file, loff_t start, loff_t end, int datasync)
// {
// 	// struct fd real;
// 	// const struct cred *old_cred;
// 	// int ret;

// 	// ret = ovl_real_fdget_meta(file, &real, !datasync);
// 	// if (ret)
// 	// 	return ret;

// 	// /* Don't sync lower file for fear of receiving EROFS error */
// 	// if (file_inode(real.file) == ovl_inode_upper(file_inode(file))) {
// 	// 	old_cred = ovl_override_creds(file_inode(file)->i_sb);
// 	// 	ret = vfs_fsync_range(real.file, start, end, datasync);
// 	// 	revert_creds(old_cred);
// 	// }

// 	// fdput(real);

// 	return 0;
// }

static loff_t ovl_llseek(struct file *file, loff_t offset, int origin)
{
	// pr_info("%s", __func__);
	// struct inode *inode = file->f_path.dentry->d_inode;
	// int retval;

	// if (origin != SEEK_DATA && origin != SEEK_HOLE)
	// 	return generic_file_llseek(file, offset, origin);
	
	// inode_lock(inode);
	// // switch(origin) {

	// // }
	if(offset != file->f_pos) {
		file->f_pos = offset;
		file->f_version = 0;
	}

	// inode_unlock(inode);
	return offset;

}

int ovl_fadvise(struct file *file, loff_t offset, loff_t len, int advice)
{
	return 0;
}

int ovl_file_open(struct inode * inode, struct file * filp)
{
	// pr_info ("Caller name: %pS\n", __builtin_return_address(0));
	// pr_info("%s, %s, pos %lu\n", __func__, filp->f_path.dentry->d_name.name, filp->f_pos);
	return generic_file_open(inode, filp);
}

const struct file_operations ovl_file_operations = {
	.open		= ovl_file_open,
	// .release	= ovl_release,
	.llseek		= ovl_llseek,
	// .llseek = generic_file_llseek,
	.read_iter	= ovl_wrap_rw_iter,
	.write_iter	= ovl_wrap_rw_iter,
    // .write_iter = generic_file_write_iter,
	// .write = ovl_dax_file_write,
	// generic_file_write_iter
	.fsync		= noop_fsync,
	// .mmap		= ovl_mmap,
	// .fallocate	= ovl_fallocate,
	//TODO BUG generic_fadvise has bug at invalidate_mapping_pages
	.fadvise	= ovl_fadvise,
	// .flush		= ovl_flush,
	// .splice_read    = generic_file_splice_read,
	// .splice_write   = ovl_splice_write,

	// .copy_file_range	= ovl_copy_file_range,
	// .remap_file_range	= ovl_remap_file_range,
};

// void ovl_tidy_log(struct ovl_sb_info *sbi)
// {
// 	// struct journal_ptr_pair *pair;
// 	struct ovl_file_write_entry *entry;
// 	int cpuid;
// 	struct inode *inode;
// 	struct super_block *sb = sbi->sb;
// 	void *kmem;

// 	// pr_info("ovl_tidy_log\n");

// 	atomic_set(&sbi->bg_signal, 0);
	
// 	// do {

// 	// 	entry = ovl_get_block(sbi->sb, pair->journal_head);
// 	// 	if(entry->write_type == 2) { // async copy on write
// 	// 		memcpy_to_pmem_nocache(NULL, (pair->journal_head + 64), entry->len);
// 	// 		pair->journal_head += 64 + ((entry->len + 63) & ~63) ;
// 	// 	} else {
// 	// 		pair->journal_head += 64;
// 	// 	}

// 	// 	PERSISTENT_BARRIER();

// 	// } while(pair->journal_head != pair->journal_tail);
// 	// pr_info("%llx, %llx\n", pair->journal_head, pair->journal_tail);
// 	// for(cpuid = 1; cpuid <=8; cpuid++) {
// 	// 	pair = ovl_get_journal_pointers(sbi->sb, cpuid);
// 	// 	// pr_info("%llx, %llx\n", pair->journal_head, pair->journal_tail);
// 	// 	while(pair->journal_head < pair->journal_tail) {
// 	// 		// pr_info("journal_head: %llx\n", pair->journal_head);
// 	// 		entry = ovl_get_block(sbi->sb, pair->journal_head);
// 	// 		// pr_info("entry type: %d\n", entry->write_type);
// 	// 		if(entry->write_type == 2) { // async copy on write
// 	// 			inode = ovl_iget(sb, entry->ino, );
// 	// 			kmem = ovl_get_page_addr(inode->i_mapping, entry->pos >> sb->s_blocksize_bits);
// 	// 			// pr_info("copy back to %llx\n", kmem);
// 	// 			memcpy_to_pmem_nocache(kmem + (entry->pos & (sb->s_blocksize - 1)), (pair->journal_head + 64), entry->len);
// 	// 			ovl_put_page(inode->i_mapping, entry->pos >> sb->s_blocksize_bits);
// 	// 			pair->journal_head += 64 + ((entry->len + 63) & ~63) ;
// 	// 		} else {
// 	// 			pair->journal_head += 64;
// 	// 		}
// 	// 	}
// 	// }
// }

