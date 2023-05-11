/*
 * ovl persistent memory management
 *
 * Copyright 2015-2016 Regents of the University of California,
 * UCSD Non-Volatile Systems Lab, Andiry Xu <jix024@cs.ucsd.edu>
 * Copyright 2012-2013 Intel Corporation
 * Copyright 2009-2011 Marco Stornelli <marco.stornelli@gmail.com>
 * Copyright 2003 Sony Corporation
 * Copyright 2003 Matsushita Electric Industrial Co., Ltd.
 * 2003-2004 (c) MontaVista Software, Inc. , Steve Longerbeam
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/fs.h>
#include <linux/bitops.h>
// #include "ovl.h"
#include "balloc.h"
#include <linux/slab.h>

int ovl_alloc_block_free_lists(struct super_block *sb)
{
	struct ovl_sb_info *sbi = OVL_SB(sb);
	struct free_list *free_list;
	int i;

	sbi->free_lists = kcalloc(sbi->cpus, sizeof(struct free_list),
				  GFP_KERNEL);

	if (!sbi->free_lists)
		return -ENOMEM;

	for (i = 0; i < sbi->cpus; i++) {
		free_list = ovl_get_free_list(sb, i);
		free_list->block_free_tree = RB_ROOT;
		spin_lock_init(&free_list->s_lock);
		free_list->index = i;
	}

	return 0;
}

void ovl_delete_free_lists(struct super_block *sb)
{
	struct ovl_sb_info *sbi = OVL_SB(sb);

	/* Each tree is freed in save_blocknode_mappings */
	kfree(sbi->free_lists);
	sbi->free_lists = NULL;
}

// static int ovl_data_csum_init_free_list(struct super_block *sb,
// 	struct free_list *free_list)
// {
// 	struct ovl_sb_info *sbi = OVL_SB(sb);
// 	unsigned long data_csum_blocks;

// 	/* Allocate pages to hold data checksums.  We store one checksum for
// 	 * each stripe for each page.  We replicate the checksums at the
// 	 * beginning and end of per-cpu region that holds the data they cover.
// 	 */
// 	data_csum_blocks = ((sbi->initsize >> ovl_STRIPE_SHIFT)
// 				* ovl_DATA_CSUM_LEN) >> PAGE_SHIFT;
// 	free_list->csum_start = free_list->block_start;
// 	free_list->block_start += data_csum_blocks / sbi->cpus;
// 	if (data_csum_blocks % sbi->cpus)
// 		free_list->block_start++;

// 	free_list->num_csum_blocks =
// 		free_list->block_start - free_list->csum_start;

// 	free_list->replica_csum_start = free_list->block_end + 1 -
// 						free_list->num_csum_blocks;
// 	free_list->block_end -= free_list->num_csum_blocks;

// 	return 0;
// }


// static int ovl_data_parity_init_free_list(struct super_block *sb,
// 	struct free_list *free_list)
// {
// 	struct ovl_sb_info *sbi = OVL_SB(sb);
// 	unsigned long blocksize, total_blocks, parity_blocks;

// 	/* Allocate blocks to store data block parity stripes.
// 	 * Always reserve in case user turns it off at init mount but later
// 	 * turns it on.
// 	 */
// 	blocksize = sb->s_blocksize;
// 	total_blocks = sbi->initsize / blocksize;
// 	parity_blocks = total_blocks / (blocksize / ovl_STRIPE_SIZE + 1);
// 	if (total_blocks % (blocksize / ovl_STRIPE_SIZE + 1))
// 		parity_blocks++;

// 	free_list->parity_start = free_list->block_start;
// 	free_list->block_start += parity_blocks / sbi->cpus;
// 	if (parity_blocks % sbi->cpus)
// 		free_list->block_start++;

// 	free_list->num_parity_blocks =
// 		free_list->block_start - free_list->parity_start;

// 	free_list->replica_parity_start = free_list->block_end + 1 -
// 		free_list->num_parity_blocks;

// 	return 0;
// }


// Initialize a free list.  Each CPU gets an equal share of the block space to
// manage.
static void ovl_init_free_list(struct super_block *sb,
	struct free_list *free_list, int index)
{
	struct ovl_sb_info *sbi = OVL_SB(sb);
	unsigned long per_list_blocks;

	per_list_blocks = (sbi->num_blocks - HEAD_RESERVED_BLOCKS - TAIL_RESERVED_BLOCKS) / sbi->cpus;

	free_list->block_start = HEAD_RESERVED_BLOCKS + per_list_blocks * index;
	free_list->block_end = free_list->block_start +
					per_list_blocks - 1;
	// if (index == 0)
	// 	free_list->block_start += sbi->head_reserved_blocks;
	// if (index == sbi->cpus - 1)
	// 	free_list->block_end -= sbi->tail_reserved_blocks;

	// ovl_data_csum_init_free_list(sb, free_list);
	// ovl_data_parity_init_free_list(sb, free_list);
}

struct ovl_range_node *ovl_alloc_blocknode(struct super_block *sb)
{
	return ovl_alloc_range_node(sb);
}

void ovl_free_blocknode(struct ovl_range_node *node)
{
	ovl_free_range_node(node);
}


void ovl_init_blockmap(struct super_block *sb, int recovery)
{
	struct ovl_sb_info *sbi = OVL_SB(sb);
	struct rb_root *tree;
	struct ovl_range_node *blknode;
	struct free_list *free_list;
	int i;
	int ret;

	/* Divide the block range among per-CPU free lists */
	sbi->per_list_blocks = (sbi->num_blocks - HEAD_RESERVED_BLOCKS - TAIL_RESERVED_BLOCKS) / sbi->cpus;
	// pr_info("per_list_blocks: %lu\n", sbi->per_list_blocks);
	for (i = 0; i < sbi->cpus; i++) {
		free_list = ovl_get_free_list(sb, i);
		tree = &(free_list->block_free_tree);
		ovl_init_free_list(sb, free_list, i);

		/* For recovery, update these fields later */
		if (recovery == 0) {
			free_list->num_free_blocks = free_list->block_end -
						free_list->block_start + 1;

			blknode = ovl_alloc_blocknode(sb);
			if (blknode == NULL)
				BUG();
			blknode->range_low = free_list->block_start;
			blknode->range_high = free_list->block_end;
			// ovl_update_range_node_checksum(blknode);
			ret = ovl_insert_blocktree(tree, blknode);
			if (ret) {
				// ovl_err(sb, "%s failed\n", __func__);
				ovl_free_blocknode(blknode);
				return;
			}
			free_list->first_node = blknode;
			free_list->last_node = blknode;
			free_list->num_blocknode = 1;
		}

		// ovl_dbgv("%s: free list %d: block start %lu, end %lu, "
		// 	  "%lu free blocks\n",
		// 	  __func__, i,
		// 	  free_list->block_start,
		// 	  free_list->block_end,
		// 	  free_list->num_free_blocks);
	}
}

static inline int ovl_rbtree_compare_rangenode(struct ovl_range_node *curr,
	unsigned long key, enum node_type type)
{
	if (type == NODE_DIR) {
		if (key < curr->hash)
			return -1;
		if (key > curr->hash)
			return 1;
		return 0;
	}

	/* Block and inode */
	if (key < curr->range_low)
		return -1;
	if (key > curr->range_high)
		return 1;

	return 0;
}

int ovl_find_range_node(struct rb_root *tree, unsigned long key,
	enum node_type type, struct ovl_range_node **ret_node)
{
	struct ovl_range_node *curr = NULL;
	struct rb_node *temp;
	int compVal;
	int ret = 0;

	temp = tree->rb_node;

	while (temp) {
		curr = container_of(temp, struct ovl_range_node, node);
		compVal = ovl_rbtree_compare_rangenode(curr, key, type);

		if (compVal == -1) {
			temp = temp->rb_left;
		} else if (compVal == 1) {
			temp = temp->rb_right;
		} else {
			ret = 1;
			break;
		}
	}

	// if (curr && !ovl_range_node_checksum_ok(curr)) {
	// 	ovl_dbg("%s: curr failed\n", __func__);
	// 	return 0;
	// }

	*ret_node = curr;
	return ret;
}


int ovl_insert_range_node(struct rb_root *tree,
	struct ovl_range_node *new_node, enum node_type type)
{
	struct ovl_range_node *curr;
	struct rb_node **temp, *parent;
	int compVal;

	temp = &(tree->rb_node);
	parent = NULL;

	while (*temp) {
		curr = container_of(*temp, struct ovl_range_node, node);
		compVal = ovl_rbtree_compare_rangenode(curr,
					new_node->range_low, type);
		parent = *temp;

		if (compVal == -1) {
			temp = &((*temp)->rb_left);
		} else if (compVal == 1) {
			temp = &((*temp)->rb_right);
		} else {
			// ovl_dbg("%s: type %d entry %lu - %lu already exists: "
			// 	"%lu - %lu\n",
			// 	 __func__, type, new_node->range_low,
			// 	new_node->range_high, curr->range_low,
			// 	curr->range_high);
			return -EINVAL;
		}
	}

	rb_link_node(&new_node->node, parent, temp);
	rb_insert_color(&new_node->node, tree);

	return 0;
}

void ovl_destroy_range_node_tree(struct super_block *sb,
	struct rb_root *tree)
{
	struct ovl_range_node *curr;
	struct rb_node *temp;

	temp = rb_first(tree);
	while (temp) {
		curr = container_of(temp, struct ovl_range_node, node);
		temp = rb_next(temp);
		rb_erase(&curr->node, tree);
		ovl_free_range_node(curr);
	}
}

int ovl_insert_blocktree(struct rb_root *tree,
	struct ovl_range_node *new_node)
{
	int ret;

	ret = ovl_insert_range_node(tree, new_node, NODE_BLOCK);
	// if (ret)
	// 	ovl_dbg("ERROR: %s failed %d\n", __func__, ret);

	return ret;
}


/* Used for both block free tree and inode inuse tree */
int ovl_find_free_slot(struct rb_root *tree, unsigned long range_low,
	unsigned long range_high, struct ovl_range_node **prev,
	struct ovl_range_node **next)
{
	struct ovl_range_node *ret_node = NULL;
	struct rb_node *tmp;
	int check_prev = 0, check_next = 0;
	int ret;

	ret = ovl_find_range_node(tree, range_low, NODE_BLOCK, &ret_node);
	if (ret) {
		// ovl_dbg("%s ERROR: %lu - %lu already in free list\n",
		// 	__func__, range_low, range_high);
		return -EINVAL;
	}

	if (!ret_node) {
		*prev = *next = NULL;
	} else if (ret_node->range_high < range_low) {
		*prev = ret_node;
		tmp = rb_next(&ret_node->node);
		if (tmp) {
			*next = container_of(tmp, struct ovl_range_node, node);
			check_next = 1;
		} else {
			*next = NULL;
		}
	} else if (ret_node->range_low > range_high) {
		*next = ret_node;
		tmp = rb_prev(&ret_node->node);
		if (tmp) {
			*prev = container_of(tmp, struct ovl_range_node, node);
			check_prev = 1;
		} else {
			*prev = NULL;
		}
	} else {
		// ovl_dbg("%s ERROR: %lu - %lu overlaps with existing "
		// 	 "node %lu - %lu\n",
		// 	 __func__, range_low, range_high, ret_node->range_low,
		// 	ret_node->range_high);
		return -EINVAL;
	}

	// if (check_prev && !ovl_range_node_checksum_ok(*prev)) {
	// 	ovl_dbg("%s: prev failed\n", __func__);
	// 	return -EIO;
	// }

	// if (check_next && !ovl_range_node_checksum_ok(*next)) {
	// 	ovl_dbg("%s: next failed\n", __func__);
	// 	return -EIO;
	// }

	return 0;
}

static int ovl_free_blocks(struct super_block *sb, unsigned long blocknr,
	int num, unsigned short btype, int log_page)
{
	struct ovl_sb_info *sbi = OVL_SB(sb);
	struct rb_root *tree;
	unsigned long block_low;
	unsigned long block_high;
	unsigned long num_blocks = 0;
	struct ovl_range_node *prev = NULL;
	struct ovl_range_node *next = NULL;
	struct ovl_range_node *curr_node;
	struct free_list *free_list;
	int cpuid;
	int new_node_used = 0;
	int ret;
	// INIT_TIMING(free_time);

	if (num <= 0) {
		// ovl_dbg("%s ERROR: free %d\n", __func__, num);
		return -EINVAL;
	}

	// ovl_START_TIMING(free_blocks_t, free_time);
	cpuid = blocknr / sbi->per_list_blocks;

	/* Pre-allocate blocknode */
	curr_node = ovl_alloc_blocknode(sb);
	if (curr_node == NULL) {
		/* returning without freeing the block*/
		// ovl_END_TIMING(free_blocks_t, free_time);
		return -ENOMEM;
	}

	free_list = ovl_get_free_list(sb, cpuid);
	spin_lock(&free_list->s_lock);

	tree = &(free_list->block_free_tree);

	num_blocks = ovl_get_numblocks(btype) * num;
	block_low = blocknr;
	block_high = blocknr + num_blocks - 1;

	// ovl_dbgv("Free: %lu - %lu\n", block_low, block_high);

	if (blocknr < free_list->block_start ||
			blocknr + num > free_list->block_end + 1) {
		// ovl_err(sb, "free blocks %lu to %lu, free list %d, "
		// 	 "start %lu, end %lu\n",
		// 	 blocknr, blocknr + num - 1,
		// 	 free_list->index,
		// 	 free_list->block_start,
		// 	 free_list->block_end);
		ret = -EIO;
		goto out;
	}

	ret = ovl_find_free_slot(tree, block_low,
					block_high, &prev, &next);

	if (ret) {
		// ovl_dbg("%s: find free slot fail: %d\n", __func__, ret);
		goto out;
	}

	if (prev && next && (block_low == prev->range_high + 1) &&
			(block_high + 1 == next->range_low)) {
		/* fits the hole */
		rb_erase(&next->node, tree);
		free_list->num_blocknode--;
		prev->range_high = next->range_high;
		// ovl_update_range_node_checksum(prev);
		if (free_list->last_node == next)
			free_list->last_node = prev;
		ovl_free_blocknode(next);
		goto block_found;
	}
	if (prev && (block_low == prev->range_high + 1)) {
		/* Aligns left */
		prev->range_high += num_blocks;
		// ovl_update_range_node_checksum(prev);
		goto block_found;
	}
	if (next && (block_high + 1 == next->range_low)) {
		/* Aligns right */
		next->range_low -= num_blocks;
		// ovl_update_range_node_checksum(next);
		goto block_found;
	}

	/* Aligns somewhere in the middle */
	curr_node->range_low = block_low;
	curr_node->range_high = block_high;
	// ovl_update_range_node_checksum(curr_node);
	new_node_used = 1;
	ret = ovl_insert_blocktree(tree, curr_node);
	if (ret) {
		new_node_used = 0;
		goto out;
	}
	if (!prev)
		free_list->first_node = curr_node;
	if (!next)
		free_list->last_node = curr_node;

	free_list->num_blocknode++;

block_found:
	free_list->num_free_blocks += num_blocks;

	if (log_page) {
		free_list->free_log_count++;
		free_list->freed_log_pages += num_blocks;
	} else {
		free_list->free_data_count++;
		free_list->freed_data_pages += num_blocks;
	}

out:
	spin_unlock(&free_list->s_lock);
	if (new_node_used == 0)
		ovl_free_blocknode(curr_node);

	// ovl_END_TIMING(free_blocks_t, free_time);
	return ret;
}

int ovl_free_data_blocks(struct super_block *sb,
	struct ovl_inode_info_header *sih, unsigned long blocknr, int num)
{
	int ret;
	// INIT_TIMING(free_time);

	// ovl_dbgv("Inode %lu: free %d data block from %lu to %lu\n",
			// sih->ino, num, blocknr, blocknr + num - 1);
	if (blocknr == 0) {
		// ovl_dbg("%s: ERROR: %lu, %d\n", __func__, blocknr, num);
		return -EINVAL;
	}
	// ovl_START_TIMING(free_data_t, free_time);
	ret = ovl_free_blocks(sb, blocknr, num, sih->i_blk_type, 0);
	if (ret) {
		// ovl_err(sb, "Inode %lu: free %d data block from %lu to %lu "
		// 	 "failed!\n",
		// 	 sih->ino, num, blocknr, blocknr + num - 1);
		// ovl_print_ovl_log(sb, sih);
	}
	// ovl_END_TIMING(free_data_t, free_time);

	return ret;
}

int ovl_free_log_blocks(struct super_block *sb,
	struct ovl_inode_info_header *sih, unsigned long blocknr, int num)
{
	int ret;
	// INIT_TIMING(free_time);

	// ovl_dbgv("Inode %lu: free %d log block from %lu to %lu\n",
	// 		sih->ino, num, blocknr, blocknr + num - 1);
	if (blocknr == 0) {
		// ovl_dbg("%s: ERROR: %lu, %d\n", __func__, blocknr, num);
		return -EINVAL;
	}
	// ovl_START_TIMING(free_log_t, free_time);
	ret = ovl_free_blocks(sb, blocknr, num, sih->i_blk_type, 1);
	if (ret) {
		// ovl_err(sb, "Inode %lu: free %d log block from %lu to %lu "
		// 	 "failed!\n",
		// 	 sih->ino, num, blocknr, blocknr + num - 1);
		// ovl_print_ovl_log(sb, sih);
	}
	// ovl_END_TIMING(free_log_t, free_time);

	return ret;
}

static int not_enough_blocks(struct free_list *free_list,
	unsigned long num_blocks, enum alloc_type atype)
{
	struct ovl_range_node *first = free_list->first_node;
	struct ovl_range_node *last = free_list->last_node;

	if (free_list->num_free_blocks < num_blocks || !first || !last) {
		// ovl_dbgv("%s: num_free_blocks=%ld; num_blocks=%ld; "
		// 	  "first=0x%p; last=0x%p",
		// 	  __func__, free_list->num_free_blocks, num_blocks,
		// 	  first, last);
		return 1;
	}

	if (atype == LOG &&
	    last->range_high - first->range_low < DEAD_ZONE_BLOCKS) {
		// ovl_dbgv("%s: allocation would cause deadzone violation. "
		// 	  "high=0x%lx, low=0x%lx, DEADZONE=%d",
		// 	  __func__, last->range_high, first->range_low,
		// 	  DEAD_ZONE_BLOCKS);
		return 1;
	}

	return 0;
}

struct ovl_range_node *ovl_alloc_blocknode_atomic(struct super_block *sb)
{
	return ovl_alloc_range_node_atomic(sb);
}

#define PAGES_PER_2MB 512
#define PAGES_PER_2MB_MASK (512 - 1)
#define IS_DATABLOCKS_2MB_ALIGNED(num_blocks, atype) \
		(!(num_blocks & PAGES_PER_2MB_MASK) && (atype == DATA))

/* This method returns the number of blocks allocated. */
// static long ovl_alloc_superpage(struct super_block *sb,
// 	struct free_list *free_list, unsigned long num_blocks,
// 	unsigned long *new_blocknr, enum ovl_alloc_direction from_tail)
// {
// 	struct rb_root *tree;
// 	struct rb_node *temp;
// 	struct ovl_range_node *curr;
// 	unsigned long curr_blocks;
// 	bool found = 0;
// 	unsigned long step = 0;

// 	unsigned int left_margin;
// 	unsigned int right_margin;

// 	tree = &(free_list->block_free_tree);
// 	if (from_tail == ALLOC_FROM_HEAD)
// 		temp = &(free_list->first_node->node);
// 	else
// 		temp = &(free_list->last_node->node);

// 	while (temp) {
// 		step++;
// 		curr = container_of(temp, struct ovl_range_node, node);

// 		if (!ovl_range_node_checksum_ok(curr)) {
// 			ovl_err(sb, "%s curr failed\n", __func__);
// 			goto next;
// 		}

// 		curr_blocks = curr->range_high - curr->range_low + 1;
// 		left_margin = PAGES_PER_2MB -
// 			(curr->range_low & PAGES_PER_2MB_MASK);

// 		/* We are doing best effort here to allocate as many 2MB blocks as possible. */
// 		if (num_blocks > (curr_blocks - left_margin)) {
// 			if (((curr_blocks - left_margin) & ~PAGES_PER_2MB_MASK) > 0)
// 				num_blocks = (curr_blocks - left_margin) & ~PAGES_PER_2MB_MASK;
// 		}

// 		/*
// 		 * Guard against cases where:
// 		 * a. Unaligned free blocks is smaller than #512
// 		 *    left_margin could larger than curr_blocks.
// 		 * b. After alignment, free blocks is smaller than
// 		 *    requested blocks.
// 		 * Otherwise, we are free to go.
// 		 */
// 		if ((curr_blocks > left_margin) && 
// 			(num_blocks <= (curr_blocks - left_margin))) {
// 			struct ovl_range_node *node;
// 			unsigned long saved_range_high = curr->range_high;

// 			*new_blocknr = curr->range_low + left_margin;
// 			right_margin = curr_blocks - left_margin - num_blocks;
// 			ovl_dbgv("curr:%p: num_blocks:%lu curr->range_low:%lu high:%lu",
// 						curr, num_blocks, curr->range_low, curr->range_high);

// 			if (left_margin) {
// 				/* Reuse "curr" and its "first_node" indicator. */
// 				curr->range_high = curr->range_low + left_margin - 1;
// 				ovl_update_range_node_checksum(curr);
// 				ovl_dbgv("Insert node for left_margin, range_low:%lu high:%lu",
// 							curr->range_low, curr->range_high);
// 			}

// 			if (right_margin) {
// 				if (left_margin) {
// 					/* curr was reused for left_margin node, grab new one. */
// 					node = ovl_alloc_blocknode_atomic(sb);
// 					if (node == NULL) {
// 						ovl_warn("Failed to allocate new block node.\n");
// 						return -ENOMEM;
// 					}
// 					node->range_low = curr->range_low + left_margin + num_blocks;
// 					node->range_high = saved_range_high;
// 					ovl_update_range_node_checksum(node);
// 					ovl_insert_blocktree(tree, node);
// 					free_list->num_blocknode++;
// 					if (curr == free_list->last_node)
// 						free_list->last_node = node;
// 				} else {
// 					/*
// 					 * curr->range_low is aligned, reuse curr for right_margin.
// 					 * Update the checksum as needed.
// 					 */
// 					curr->range_low = curr->range_low + num_blocks;
// 					ovl_update_range_node_checksum(curr);
// 				}
// 				ovl_dbgv("Insert node for right_margin, range_low:%lu high:%lu",
// 							node->range_low, node->range_high);
// 			}

// 			/* Catch up special case where curr is aligned and used up. */
// 			if (!left_margin && !right_margin) {

// 				/* corner case in corner, spotted by Andiry. */
// 				node = NULL;
// 				if (curr == free_list->first_node) {
// 					temp = rb_next(temp);
// 					if (temp)
// 						node = container_of(temp, struct ovl_range_node, node);
// 					free_list->first_node = node;
// 				}
// 				if (curr == free_list->last_node) {
// 					temp = rb_prev(temp);
// 					if (temp)
// 						node = container_of(temp, struct ovl_range_node, node);
// 					free_list->last_node = node;
// 				}

// 				/* release curr after updating {first, last}_node */
// 				rb_erase(&curr->node, tree);
// 				ovl_free_blocknode(curr);
// 				free_list->num_blocknode--;
// 			}

// 			found = 1;
// 			break;
// 		}
// next:
// 		if (from_tail == ALLOC_FROM_HEAD)
// 			temp = rb_next(temp);
// 		else
// 			temp = rb_prev(temp);
// 	}

// 	ovl_STATS_ADD(alloc_steps, step);
// 	return found ? num_blocks : 0;
// }

/* Return how many blocks allocated */
static long ovl_alloc_blocks_in_free_list(struct super_block *sb,
	struct free_list *free_list, unsigned short btype,
	enum alloc_type atype, unsigned long num_blocks,
	unsigned long *new_blocknr, enum ovl_alloc_direction from_tail)
{
	struct rb_root *tree;
	struct ovl_range_node *curr, *next = NULL, *prev = NULL;
	struct rb_node *temp, *next_node, *prev_node;
	unsigned long curr_blocks;
  	// long ret_blocks = 0;
	bool found = 0;
	bool found_hugeblock = 0;
	unsigned long step = 0;

	if (!free_list->first_node || free_list->num_free_blocks == 0) {
		// ovl_dbgv("%s: Can't alloc. free_list->first_node=0x%p "
		// 	  "free_list->num_free_blocks = %lu",
		// 	  __func__, free_list->first_node,
		// 	  free_list->num_free_blocks);
		return -ENOSPC;
	}

	if (atype == LOG && not_enough_blocks(free_list, num_blocks, atype)) {
		// ovl_dbgv("%s: Can't alloc.  not_enough_blocks() == true",
		// 	  __func__);
		return -ENOSPC;
	}

	tree = &(free_list->block_free_tree);
	if (from_tail == ALLOC_FROM_HEAD)
		temp = &(free_list->first_node->node);
	else
		temp = &(free_list->last_node->node);

	/* Try huge block allocation for data blocks first */
	// if (IS_DATABLOCKS_2MB_ALIGNED(num_blocks, atype)) {
	// 	ret_blocks = ovl_alloc_superpage(sb, free_list,
	// 				num_blocks, new_blocknr, from_tail);
	// 	if (ret_blocks > 0 && *new_blocknr != 0) {
	// 		num_blocks = ret_blocks;
	// 		found_hugeblock = 1;
	// 		goto success;
	// 	}
	// }

	/* fallback to un-aglined allocation then */
	while (temp) {
		step++;
		curr = container_of(temp, struct ovl_range_node, node);

		// if (!ovl_range_node_checksum_ok(curr)) {
		// 	// ovl_err(sb, "%s curr failed\n", __func__);
		// 	goto next;
		// }

		curr_blocks = curr->range_high - curr->range_low + 1;

		if (num_blocks >= curr_blocks) {
			/* Superpage allocation must succeed */
			if (btype > 0 && num_blocks > curr_blocks)
				goto next;

			/* Otherwise, allocate the whole blocknode */
			if (curr == free_list->first_node) {
				next_node = rb_next(temp);
				if (next_node)
					next = container_of(next_node,
						struct ovl_range_node, node);
				free_list->first_node = next;
			}

			if (curr == free_list->last_node) {
				prev_node = rb_prev(temp);
				if (prev_node)
					prev = container_of(prev_node,
						struct ovl_range_node, node);
				free_list->last_node = prev;
			}

			rb_erase(&curr->node, tree);
			free_list->num_blocknode--;
			num_blocks = curr_blocks;
			*new_blocknr = curr->range_low;
			ovl_free_blocknode(curr);
			found = 1;
			break;
		}

		/* Allocate partial blocknode */
		if (from_tail == ALLOC_FROM_HEAD) {
			*new_blocknr = curr->range_low;
			curr->range_low += num_blocks;
		} else {
			*new_blocknr = curr->range_high + 1 - num_blocks;
			curr->range_high -= num_blocks;
		}

		// ovl_update_range_node_checksum(curr);
		found = 1;
		break;
next:
		if (from_tail == ALLOC_FROM_HEAD)
			temp = rb_next(temp);
		else
			temp = rb_prev(temp);
	}

	if (free_list->num_free_blocks < num_blocks) {
		// ovl_dbg("%s: free list %d has %lu free blocks, "
		// 	 "but allocated %lu blocks?\n",
		// 	 __func__, free_list->index,
		// 	 free_list->num_free_blocks, num_blocks);
		return -ENOSPC;
	}

// success:
	if ((found == 1) || (found_hugeblock == 1))
		free_list->num_free_blocks -= num_blocks;
	else {
		// ovl_dbgv("%s: Can't alloc.  found = %d", __func__, found);
		return -ENOSPC;
	}

	// ovl_STATS_ADD(alloc_steps, step);

	return num_blocks;
}

/* Find out the free list with most free blocks */
static int ovl_get_candidate_free_list(struct super_block *sb)
{
	struct ovl_sb_info *sbi = OVL_SB(sb);
	struct free_list *free_list;
	int cpuid = 0;
	int num_free_blocks = 0;
	int i;

	for (i = 0; i < sbi->cpus; i++) {
		free_list = ovl_get_free_list(sb, i);
		if (free_list->num_free_blocks > num_free_blocks) {
			cpuid = i;
			num_free_blocks = free_list->num_free_blocks;
		}
	}

	return cpuid;
}

static int ovl_new_blocks(struct super_block *sb, unsigned long *blocknr,
	unsigned int num, unsigned short btype, int zero,
	enum alloc_type atype, int cpuid, enum ovl_alloc_direction from_tail)
{
	struct free_list *free_list;
	void *bp;
	unsigned long num_blocks = 0;
	unsigned long new_blocknr = 0;
	long ret_blocks = 0;
	int retried = 0;
	unsigned long irq_flags = 0;
	// INIT_TIMING(alloc_time);

	num_blocks = num * ovl_get_numblocks(btype);
	if (num_blocks == 0) {
		// ovl_dbg_verbose("%s: num_blocks == 0", __func__);
		return -EINVAL;
	}

	// ovl_START_TIMING(new_blocks_t, alloc_time);
	if (cpuid == ANY_CPU)
		cpuid = ovl_get_cpuid(sb);
	// cpuid = 0;

retry:
	free_list = ovl_get_free_list(sb, cpuid);
	spin_lock(&free_list->s_lock);


	if (not_enough_blocks(free_list, num_blocks, atype)) {

		pr_info("%s: cpu %d, free_blocks %lu, required %lu, "
			  "blocknode %lu\n",
			  __func__, cpuid, free_list->num_free_blocks,
			  num_blocks, free_list->num_blocknode);

		if (retried >= 2)
			/* Allocate anyway */
			goto alloc;

		spin_unlock(&free_list->s_lock);
		cpuid = ovl_get_candidate_free_list(sb);
		retried++;
		goto retry;
	}
alloc:
	ret_blocks = ovl_alloc_blocks_in_free_list(sb, free_list, btype, atype,
					num_blocks, &new_blocknr, from_tail);

	if (ret_blocks > 0) {
		if (atype == LOG) {
			free_list->alloc_log_count++;
			free_list->alloc_log_pages += ret_blocks;
		} else if (atype == DATA) {
			free_list->alloc_data_count++;
			free_list->alloc_data_pages += ret_blocks;
		}
	}

	spin_unlock(&free_list->s_lock);
	// ovl_END_TIMING(new_blocks_t, alloc_time);

	if (ret_blocks <= 0 || new_blocknr == 0) {
		pr_info("%s: not able to allocate %d blocks. "
			  "ret_blocks=%ld; new_blocknr=%lu",
			  __func__, num, ret_blocks, new_blocknr);
		return -ENOSPC;
	}

	if (zero) {
		bp = ovl_get_block(sb, ovl_get_block_off(sb,
						new_blocknr, btype));
		ovl_memunlock_range(sb, bp, PAGE_SIZE * ret_blocks, &irq_flags);
		memset_nt(bp, 0, PAGE_SIZE * ret_blocks);
		ovl_memlock_range(sb, bp, PAGE_SIZE * ret_blocks, &irq_flags);
	}
	*blocknr = new_blocknr;

	// ovl_dbg_verbose("Alloc %lu NVMM blocks 0x%lx\n", ret_blocks, *blocknr);
	return ret_blocks / ovl_get_numblocks(btype);
}

// Allocate data blocks.  The offset for the allocated block comes back in
// blocknr.  Return the number of blocks allocated.
int ovl_new_data_blocks(struct super_block *sb,
	struct ovl_inode_info_header *sih, unsigned long *blocknr,
	unsigned long start_blk, unsigned int num,
	enum ovl_alloc_init zero, int cpu,
	enum ovl_alloc_direction from_tail)
{
	int allocated;
	// INIT_TIMING(alloc_time);

	// ovl_START_TIMING(new_data_blocks_t, alloc_time);
	allocated = ovl_new_blocks(sb, blocknr, num,
			    0, zero, DATA, cpu, from_tail);
	// ovl_END_TIMING(new_data_blocks_t, alloc_time);
	if (allocated < 0) {
		pr_info("FAILED: Inode %lu, start blk %lu, "
			  "alloc %d data blocks from %lu to %lu\n",
			  1UL, start_blk, allocated, *blocknr,
			  *blocknr + allocated - 1);
	} else {
		// pr_info("Inode %lu, start blk %lu, "
		// 	  "alloc %d data blocks from %lu to %lu\n",
		// 	  1, start_blk, allocated, *blocknr,
		// 	  *blocknr + allocated - 1);
	}
	return allocated;
}


// Allocate log blocks.	 The offset for the allocated block comes back in
// blocknr.  Return the number of blocks allocated.
// int ovl_new_log_blocks(struct super_block *sb,
// 			struct ovl_inode_info_header *sih,
// 			unsigned long *blocknr, unsigned int num,
// 			enum ovl_alloc_init zero, int cpu,
// 			enum ovl_alloc_direction from_tail)
// {
// 	int allocated;
// 	// INIT_TIMING(alloc_time);

// 	// ovl_START_TIMING(new_log_blocks_t, alloc_time);
// 	allocated = ovl_new_blocks(sb, blocknr, num,
// 			    sih->i_blk_type, zero, LOG, cpu, from_tail);
// 	// ovl_END_TIMING(new_log_blocks_t, alloc_time);
// 	if (allocated < 0) {
// 		// ovl_dbgv("%s: ino %lu, failed to alloc %d log blocks",
// 			//   __func__, sih->ino, num);
// 	} else {
// 		// ovl_dbgv("%s: ino %lu, alloc %d of %d log blocks %lu to %lu\n",
// 		// 	  __func__, sih->ino, allocated, num, *blocknr,
// 		// 	  *blocknr + allocated - 1);
// 	}
// 	return allocated;
// }

// unsigned long ovl_count_free_blocks(struct super_block *sb)
// {
// 	struct ovl_sb_info *sbi = OVL_SB(sb);
// 	struct free_list *free_list;
// 	unsigned long num_free_blocks = 0;
// 	int i;

// 	for (i = 0; i < sbi->cpus; i++) {
// 		free_list = ovl_get_free_list(sb, i);
// 		num_free_blocks += free_list->num_free_blocks;
// 	}

// 	return num_free_blocks;
// }


