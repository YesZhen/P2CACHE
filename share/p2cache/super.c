#include <uapi/linux/magic.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/xattr.h>
#include <linux/mount.h>
#include <linux/parser.h>
#include <linux/module.h>
#include <linux/statfs.h>
#include <linux/seq_file.h>
#include <linux/posix_acl_xattr.h>
#include <linux/exportfs.h>
#include "overlayfs.h"
#include "balloc.h"

#include <uapi/linux/mount.h>
#include <linux/vmalloc.h>
#include <linux/dax.h>
#include <linux/blkdev.h>
#include <linux/pfn_t.h>
#include <linux/dcache.h>

#include "stats.h"

MODULE_AUTHOR("Zhen Lin");
MODULE_DESCRIPTION("p2cache");
MODULE_LICENSE("GPL");

int ovl_support_clwb;

static struct kmem_cache *pcache_inode_cachep;
static struct kmem_cache *pcache_range_node_cachep;

static int __init init_rangenode_cache(void)
{
	pcache_range_node_cachep = kmem_cache_create("pcache_range_node_cache",
					sizeof(struct ovl_range_node),
					0, (SLAB_RECLAIM_ACCOUNT |
					SLAB_MEM_SPREAD), NULL);
	if (pcache_range_node_cachep == NULL)
		return -ENOMEM;
	return 0;
}

struct ovl_range_node *ovl_alloc_range_node_atomic(struct super_block *sb)
{
	struct ovl_range_node *p;

	p = (struct ovl_range_node *)
		kmem_cache_zalloc(pcache_range_node_cachep, GFP_ATOMIC);
	return p;
}

struct ovl_range_node *ovl_alloc_range_node(struct super_block *sb)
{
	struct ovl_range_node *p;

	p = (struct ovl_range_node *)
		kmem_cache_zalloc(pcache_range_node_cachep, GFP_NOFS);
	return p;
}


void ovl_free_range_node(struct ovl_range_node *node)
{
	kmem_cache_free(pcache_range_node_cachep, node);
}

enum {
	Opt_init, Opt_err
};

static const match_table_t tokens = {
	{ Opt_init,	     "init"		  },
	{ Opt_err,	NULL},
};

static int pcache_parse_options(char *options, struct ovl_sb_info *sbi,
			       bool remount)
{
	char *p;
	substring_t args[MAX_OPT_ARGS];
	int option;
	kuid_t uid;

	if (!options)
		return 0;

	while ((p = strsep(&options, ",")) != NULL) {
		int token;

		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_init:
			if (remount)
				goto bad_opt;
			set_opt(sbi->s_mount_opt, FORMAT);
			break;
		default: {
			goto bad_opt;
		}
		}
	}

	return 0;

bad_val:
	pr_info("Bad value '%s' for mount option '%s'\n", args[0].from,
	       p);
	return -EINVAL;
bad_opt:
	pr_info("Bad mount option: \"%s\"\n", p);
	return -EINVAL;
}

static inline void set_default_opts(struct ovl_sb_info *sbi)
{
	set_opt(sbi->s_mount_opt, HUGEIOREMAP);
	set_opt(sbi->s_mount_opt, ERRORS_CONT);
	sbi->head_reserved_blocks = HEAD_RESERVED_BLOCKS;
	sbi->tail_reserved_blocks = TAIL_RESERVED_BLOCKS;
	// sbi->cpus = num_online_cpus();
	sbi->cpus = 12;
	ovl_info("%d cpus online\n", sbi->cpus);
	sbi->map_id = 0;
	sbi->enable_dram_page = 1;
	sbi->enable_dram_log = 0;
	sbi->write_back_sync = 1;
	// sbi->snapshot_si = NULL;
}


int ovl_lite_journal_soft_init(struct super_block *sb)
{
	struct ovl_sb_info *sbi = sb->s_fs_info;
	// struct journal_ptr_pair *pair;
	// int i;
	int ret = 0;

	// sbi->journal_locks = kcalloc(1, sizeof(spinlock_t), GFP_KERNEL);
	// if (!sbi->journal_locks)
	// 	return -ENOMEM;

	
	// spin_lock_init(sbi->journal_locks);


	return ret;
}

static inline void calculate_and_print_bandwidth(struct timespec *begin, struct timespec *end) {
    unsigned long duration = (end->tv_sec - begin->tv_sec) * 1000000000UL + (end->tv_nsec - begin->tv_nsec);
    // pr_info("sz: %lu, duration: %lu nanosecond, bw: %lu GB/s\n", access_size, duration ,  total_size / duration);
    pr_info("duration: %lu nanosecond\n", duration);
}

int ovl_rebuild_dentry_cache(struct super_block *sb, void* pmem_addr)
{
	struct ovl_sb_info *sbi = sb->s_fs_info;
	struct journal_ptr_pair *pair;
	int i;
	u64 block;
	unsigned long percpu_log_blocks = HEAD_RESERVED_BLOCKS / sbi->cpus;

	struct timespec begin, end;

	char buf[256], log_buff[512];
	struct path path;
	
	struct dentry *dentry;
	struct inode *inode;

	void* addr = pmem_addr;
	
	
	
	// struct qstr q;

	// q.name = "aaa";
	// q.hash_len = hashlen_string(sb->s_root, q.name);

	for(i = 0; i < 1000000; i++) {
		sprintf(buf, "file-%d", i);
		memcpy(addr, buf, 256);
		addr += 512;
	}

	addr = pmem_addr;
	getrawmonotonic(&begin);
	for(i = 0; i < 10000000; i++) {
		memcpy(log_buff, addr, 512);
		dentry = d_alloc_name(sb->s_root, log_buff);
		inode = pcache_get_inode(sb, NULL, S_IFREG | 0755, 0, 0);
		d_add(dentry, inode);
		addr += 512;
	}
	getrawmonotonic(&end);

	calculate_and_print_bandwidth(&begin, &end);
	

	// for(i = 0; i < sbi->cpus; i++) {
	// 	pair = ovl_get_dir_journal_pointers(sb, i);

	// 	if(pair) {
	// 		pr_info("cpu %d, head %lx, tail %lx\n", pair->journal_head, pair->journal_tail);
	// 	}
	// }

	// for(i = 0; i < 10; i++) {
	// 	memset(&path, 0, sizeof(path));
	// 	pr_info("%lu", i);
	// 	snprintf(buf, 256, "/mnt/data/rebuild-%d", i);
	// 	dentry = kern_path_create(AT_FDCWD, buf, &path, 0);
	// 	pr_info("%lx", dentry);
	// 	if (IS_ERR(dentry))
	// 		pr_info("PTR_ERR %ld", PTR_ERR(dentry));
	// 	else
	// 		vfs_create(path.dentry->d_inode, dentry, S_IFREG, true);
	// }

	return 0;
}

int ovl_lite_journal_hard_init(struct super_block *sb)
{
	struct ovl_sb_info *sbi = sb->s_fs_info;
	// struct ovl_inode_info_header sih;
	struct journal_ptr_pair *pair;
	// unsigned long blocknr = 0;
	int i;
	u64 block;
	// unsigned long head_dir_log_reserved_blocks = (HEAD_LOG_RESERVED_BLOCKS / 4) - 64;
	// unsigned long head_file_log_reserved_blocks = HEAD_LOG_RESERVED_BLOCKS - (HEAD_LOG_RESERVED_BLOCKS / 4);
	// unsigned long percpu_log_blocks = (HEAD_RESERVED_BLOCKS - 64) / sbi->cpus;
	// unsigned long percpu_dir_log_blocks = percpu_log_blocks / 4;
	// unsigned long percpu_file_log_blocks = percpu_log_blocks - percpu_dir_log_blocks;
	unsigned long percpu_log_blocks = HEAD_RESERVED_BLOCKS / sbi->cpus;
	pr_info("%s", __func__);
	for(i = 0; i < sbi->cpus; i++) {
		
		pair = ovl_get_dir_journal_pointers(sb, i);
		block = ovl_get_block_off(sb, i * percpu_log_blocks, 0);
		
		if(sbi->cpus == 0)
			block += 64;
		
		// pair->journal_head = pair->journal_tail = block;
		// pr_info("pair->journal_head: %llx, pair->journal_tail: %llx\n", pair->journal_head, pair->journal_tail);
		pair->journal_head = block;
		pair->journal_tail = block;
		
		ovl_flush_buffer(pair, CACHELINE_SIZE, 0);
		
		pair = ovl_get_journal_pointers(sb, i);
		block = ovl_get_block_off(sb, (i * percpu_log_blocks) + (percpu_log_blocks>>2), 0);	
		// pair->journal_head = pair->journal_tail = block;
		pair->journal_head = block;
		pair->journal_tail = block;
		ovl_flush_buffer(pair, CACHELINE_SIZE, 0);
	}

	PERSISTENT_BARRIER();
	return ovl_lite_journal_soft_init(sb);
}

static struct inode *ovl_alloc_inode(struct super_block *sb)
{
	struct ovl_inode_info *oi = kmem_cache_alloc(pcache_inode_cachep, GFP_KERNEL);
	if (!oi)
		return NULL;

	xa_init(&oi->header.pmem_pages);
	xa_init(&oi->header.dram_pages);
	// oi->header.log_index = RB_ROOT;
	xa_init(&oi->header.log_index);
	xa_init(&oi->header.partial_writes);
	
	
	// oi->cache = NULL;
	// oi->redirect = NULL;
	// oi->version = 0;
	// oi->flags = 0;
	// oi->__upperdentry = NULL;
	// oi->lower = NULL;
	// oi->lowerdata = NULL;
	// mutex_init(&oi->lock);

	

	return &oi->vfs_inode;
}

//ovl_delete_free_lists(sb);
// kfree(sbi->zeroed_page);

static void ovl_put_super(struct super_block *sb)
{
	struct ovl_sb_info *sbi = sb->s_fs_info;

	if(sbi->dram_pool)
		vfree(sbi->dram_pool);

	if (sbi->virt_addr) {
		sbi->virt_addr = NULL;
	}
	rcu_barrier();
	ovl_delete_free_lists(sb);
	rcu_barrier();

	kfree(sbi->zeroed_page);
	rcu_barrier();
	fs_put_dax(sbi->s_dax_dev);
	rcu_barrier();
	
	kfree(sbi);
	rcu_barrier();

}

// static void ovl_i_callback(struct rcu_head *head)
// {
// 	struct inode *inode = container_of(head, struct inode, i_rcu);
// 	// struct ovl_inode_info *vi = ovl_I(inode);

// 	// ovl_dbg_verbose("%s: ino %lu\n", __func__, inode->i_ino);
// 	// kmem_cache_free(ovl_inode_cachep, vi);
// }

// static void ovl_destroy_inode(struct inode *inode)
// {
// 	// ovl_dbgv("%s: %lu\n", __func__, inode->i_ino);
// 	call_rcu(&inode->i_rcu, ovl_i_callback);
// }

void ovl_evict_inode(struct inode *inode)
{
	clear_inode(inode);
}

int ovl_statfs(struct dentry *dentry, struct kstatfs *buf) {
	int ret;
	ret = simple_statfs(dentry, buf);
	buf->f_blocks = 0x2000000000;
	buf->f_bfree = 0x1000000000;
	buf->f_bavail = 0x1000000000;
	buf->f_ffree = 0x1000000000;
	buf->f_files = 0x2000000000;
	return ret;
}

static void destroy_inodecache(void)
{
	/*
	 * Make sure all delayed rcu free inodes are flushed before
	 * we destroy cache.
	 */
	rcu_barrier();
	kmem_cache_destroy(pcache_inode_cachep);
}

static void destroy_rangenode_cache(void)
{
	kmem_cache_destroy(pcache_range_node_cachep);
}

static void pcache_i_callback(struct rcu_head *head)
{
	struct inode *inode = container_of(head, struct inode, i_rcu);
	struct ovl_inode_info *vi = OVL_I(inode);

	// nova_dbg_verbose("%s: ino %lu\n", __func__, inode->i_ino);
	kmem_cache_free(pcache_inode_cachep, vi);
}

static void pcache_destroy_inode(struct inode *inode)
{
	// nova_dbgv("%s: %lu\n", __func__, inode->i_ino);
	call_rcu(&inode->i_rcu, pcache_i_callback);
}

static const struct super_operations ovl_super_operations = {
	.alloc_inode	= ovl_alloc_inode,
	// .free_inode	= ovl_free_inode,
	.destroy_inode	= pcache_destroy_inode,
	// .evict_inode = ovl_evict_inode,
	// .drop_inode	= generic_delete_inode,
	.put_super	= ovl_put_super,
	// .sync_fs	= ovl_sync_fs,
	.statfs		= ovl_statfs,
	// .show_options	= ovl_show_options,
	// .remount_fs	= ovl_remount,
};

static struct dentry *ovl_get_root(struct super_block *sb)
{
	struct dentry *root;

	// root = d_make_root(ovl_new_inode(sb, S_IFDIR, 0));
	root = d_make_root(ovl_iget(sb, 1, S_IFDIR));
	if (!root)
		return NULL;

	return root;
}

static int ovl_get_nvmm_info(struct super_block *sb,
	struct ovl_sb_info *sbi)
{
	void* virt_addr;
	pfn_t __pfn_t;
	struct dax_device *dax_dev;
	unsigned long num_blocks;
	void *tmp_buf;
	unsigned long pos, step;

	// virt_addr = vmalloc(1024UL * 1024UL * 1024UL * 8UL);

	dax_dev = fs_dax_get_by_bdev(sb->s_bdev);
	if(!dax_dev)
		pr_err("No dax_dev\n");
	
	num_blocks = dax_direct_access(dax_dev, 0, LONG_MAX/PAGE_SIZE, &virt_addr, &__pfn_t);

	pr_info("num_blocks: %lu\n", num_blocks);

	sbi->initsize = num_blocks * PAGE_SIZE;

	// sbi->initsize = dax_direct_access(fs_dax_get_by_bdev(sb->s_bdev), 0, LONG_MAX / PAGE_SIZE,
	// 			 &virt_addr, &__pfn_t) * PAGE_SIZE;
	pr_info("page size:%lu\n", PAGE_SIZE);
	pr_info("size: %lu\n", sbi->initsize);

	sbi->virt_addr = virt_addr;
	pr_info("virt_addr: %llx\n", sbi->virt_addr);
	// sbi->initsize = 1024UL * 1024UL * 1024UL * 8UL;

	// tmp_buf = kzalloc(4096, GFP_KERNEL);
	// step = 4096;
	// for(pos = 0; pos + step <= sbi->initsize; pos += step) {
	// 	memcpy(tmp_buf, virt_addr + pos, 64);
	// }
	return 0;
}

static void bg_try_sleeping(struct ovl_sb_info *sbi)
{
	DEFINE_WAIT(wait);

	prepare_to_wait(&sbi->bg_wait, &wait, TASK_INTERRUPTIBLE);
	schedule();
	finish_wait(&sbi->bg_wait, &wait);
}

// static int ovl_bg(void *arg)
// {
// 	int i = 0;
// 	struct ovl_sb_info *sbi = arg;

// 	// pr_info("ovl_bg\n");

// 	for (;;) {
// 		if(!atomic_read(&sbi->bg_signal)) {
// 			for(i = 0; i < 10000000; i++)
// 				if(atomic_read(&sbi->bg_signal))
// 					goto tidy;
// 			// pr_info("try sleeping\n");
// 			bg_try_sleeping(sbi);
// 			// pr_info("bg awake\n");
// 		}
// 		if (kthread_should_stop())
// 			break;

// tidy:
// 		// nova_clean_snapshot(sbi);
// 		ovl_tidy_log(sbi);

// 	}

	
		

// 	return 0;
// }

// static int ovl_bg_thread_init(struct ovl_sb_info *sbi)
// {
// 	int ret = 0;

// 	atomic_set(&sbi->bg_signal, 0);

// 	init_waitqueue_head(&sbi->bg_wait);

// 	sbi->bg_thread = kthread_run(ovl_bg,
// 		sbi, "ovl_bg");
// 	if (IS_ERR(sbi->bg_thread)) {
// 		ret = -1;
// 	}
// 	return ret;
// }

static void ovl_unescape(char *s)
{
	char *d = s;

	for (;; s++, d++) {
		if (*s == '\\')
			s++;
		*d = *s;
		if (!*s)
			break;
	}
}

bool ovl_dentry_weird(struct dentry *dentry)
{
	return dentry->d_flags & (DCACHE_NEED_AUTOMOUNT |
				  DCACHE_MANAGE_TRANSIT |
				  DCACHE_OP_HASH |
				  DCACHE_OP_COMPARE);
}

static int ovl_mount_dir_noesc(const char *name, struct path *path)
{
	int err = -EINVAL;

	if (!*name) {
		pr_err("overlayfs: empty lowerdir\n");
		goto out;
	}
	err = kern_path(name, LOOKUP_FOLLOW, path);
	if (err) {
		pr_err("overlayfs: failed to resolve '%s': %i\n", name, err);
		goto out;
	}
	err = -EINVAL;
	if (ovl_dentry_weird(path->dentry)) {
		pr_err("overlayfs: filesystem on '%s' not supported\n", name);
		goto out_put;
	}
	if (!d_is_dir(path->dentry)) {
		pr_err("overlayfs: '%s' not a directory\n", name);
		goto out_put;
	}
	return 0;

out_put:
	path_put_init(path);
out:
	return err;
}

bool ovl_dentry_remote(struct dentry *dentry)
{
	return dentry->d_flags &
		(DCACHE_OP_REVALIDATE | DCACHE_OP_WEAK_REVALIDATE |
		 DCACHE_OP_REAL);
}

static int ovl_mount_dir(const char *name, struct path *path)
{
	int err = -ENOMEM;
	char *tmp = kstrdup(name, GFP_KERNEL);

	if (tmp) {
		ovl_unescape(tmp);
		err = ovl_mount_dir_noesc(tmp, path);

		if (!err) {
			if ((path->dentry->d_sb->s_magic != 0xABCDEF) && ovl_dentry_remote(path->dentry)) {
				pr_err("overlayfs: filesystem on '%s' not supported as upperdir\n",
				       tmp);
				path_put_init(path);
				err = -EINVAL;
			}
		}
		kfree(tmp);
	}
	return err;
}

bool ovl_inuse_trylock(struct dentry *dentry)
{
	struct inode *inode = d_inode(dentry);
	bool locked = false;

	spin_lock(&inode->i_lock);
	if (!(inode->i_state & I_OVL_INUSE)) {
		inode->i_state |= I_OVL_INUSE;
		locked = true;
	}
	spin_unlock(&inode->i_lock);

	return locked;
}

static int ovl_check_namelen(struct path *path, struct ovl_fs *ofs,
			     const char *name)
{
	struct kstatfs statfs;
	int err = vfs_statfs(path, &statfs);

	if (err)
		pr_err("overlayfs: statfs failed on '%s'\n", name);
	else
		ofs->namelen = max(ofs->namelen, statfs.f_namelen);

	return err;
}

static int ovl_report_in_use(struct ovl_fs *ofs, const char *name)
{
	if (ofs->config.index) {
		pr_err("overlayfs: %s is in-use as upperdir/workdir of another mount, mount with '-o index=off' to override exclusive upperdir protection.\n",
		       name);
		return -EBUSY;
	} else {
		pr_warn("overlayfs: %s is in-use as upperdir/workdir of another mount, accessing files from both mounts will result in undefined behavior.\n",
			name);
		return 0;
	}
}

static int ovl_get_upper(struct super_block *sb, struct ovl_fs *ofs,
			 struct path *upperpath)
{
	struct vfsmount *upper_mnt;
	int err;

	err = ovl_mount_dir(ofs->config.upperdir, upperpath);
	if (err)
		goto out;

	/* Upper fs should not be r/o */
	if (sb_rdonly(upperpath->mnt->mnt_sb)) {
		pr_err("overlayfs: upper fs is r/o, try multi-lower layers mount\n");
		err = -EINVAL;
		goto out;
	}

	err = ovl_check_namelen(upperpath, ofs, ofs->config.upperdir);
	if (err)
		goto out;

	// err = ovl_setup_trap(sb, upperpath->dentry, &ofs->upperdir_trap,
	// 		     "upperdir");
	// if (err)
	// 	goto out;

	upper_mnt = clone_private_mount(upperpath);
	err = PTR_ERR(upper_mnt);
	if (IS_ERR(upper_mnt)) {
		pr_err("overlayfs: failed to clone upperpath\n");
		goto out;
	}

	/* Don't inherit atime flags */
	upper_mnt->mnt_flags &= ~(MNT_NOATIME | MNT_NODIRATIME | MNT_RELATIME);
	ofs->upper_mnt = upper_mnt;

	if (ovl_inuse_trylock(ofs->upper_mnt->mnt_root)) {
		ofs->upperdir_locked = true;
	} else {
		err = ovl_report_in_use(ofs, "upperdir");
		if (err)
			goto out;
	}

	err = 0;
out:
	return err;
}


static int ovl_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *root_inode;
	struct ovl_sb_info *sbi = NULL;
	int retval = -EINVAL;
	int m, i;

	// struct open_flags op;

	struct ovl_fs ovlfs = {
		.config = {
			.upperdir = "/mnt/sdb"
		}
	};

	struct ovl_fs *ofs = &ovlfs;

	// struct path upperpath;

	struct path underlying = { };

	

	// sb->s_magic = 0xABCDEF;
	// sb->s_op = &ovl_super_operations;
	// sb->s_xattr = NULL;
	// sb->s_flags |= MS_NOSEC;

	// int m = do_mount("/dev/sdb", "/mnt/sdb", "ext2", 0, NULL);

	// pr_info("do_mount: %d\n");

	sbi = kzalloc(sizeof(struct ovl_sb_info), GFP_KERNEL);
	if (!sbi)
		return -ENOMEM;
	
	sb->s_fs_info = sbi;
	sbi->sb = sb;

	set_default_opts(sbi);

	/* Currently the log page supports 64 journal pointer pairs */
	if (sbi->cpus > MAX_CPUS) {
		pcache_err(sb, "ovl needs more log pointer pages to support more than "
			  __stringify(MAX_CPUS) " cpus.\n");
		goto out;
	}

	ovl_get_nvmm_info(sb, sbi);

	sbi->mode = (0755);
	sbi->uid = current_fsuid();
	sbi->gid = current_fsgid();
	set_opt(sbi->s_mount_opt, HUGEIOREMAP);

	mutex_init(&sbi->vma_mutex);
	INIT_LIST_HEAD(&sbi->mmap_sih_list);

	mutex_init(&sbi->s_lock);

	sbi->zeroed_page = kzalloc(PAGE_SIZE, GFP_KERNEL);
	sbi->bio_buffer = kzalloc(PAGE_SIZE * 16, GFP_KERNEL);

	sbi->num_blocks = ((unsigned long)(sbi->initsize) >> PAGE_SHIFT);
	pr_info("num_blocks: %lu\n", sbi->num_blocks);


	retval = pcache_parse_options(data, sbi, 0);
	if (retval) {
		pcache_err(sb, "%s: Failed to parse pcache command line options.",
			 __func__);
		goto out;
	}

	if (ovl_alloc_block_free_lists(sb)) {
		retval = -ENOMEM;
		pcache_err(sb, "%s: Failed to allocate block free lists.",
			 __func__);
		goto out;
	}

	// ovl_sysfs_init(sb);

	
	

	

	// ovl_bg_thread_init(sbi);

	sb->s_blocksize = PAGE_SIZE;
	sb->s_blocksize_bits = PAGE_SHIFT;

	// sb->s_magic = le32_to_cpu(sbi->ovl_sb->s_magic);
	sb->s_magic = 0xABCDEF;
	sb->s_op = &ovl_super_operations;
	// sb->s_maxbytes = ovl_max_size(sb->s_blocksize_bits);
	sb->s_time_gran = 1000000000; // 1 second.
	// sb->s_export_op = &ovl_export_ops;
	sb->s_xattr = NULL;
	sb->s_flags |= MS_NOSEC;




	// root_inode = ovl_iget(sb, 1, S_IFDIR);
	root_inode = pcache_get_inode(sb, NULL, S_IFDIR | 0755, 0, 1);
	// ramfs_get_inode(sb, NULL, S_IFDIR | fsi->mount_opts.mode, 0);
	// inode_init_owner(&init_user_ns, root_inode, NULL, root_inode->i_mode);
	
	
	sb->s_root = d_make_root(root_inode);


	// if (sbi->s_mount_opt & PCACHE_MOUNT_FORMAT) {
	// 	// root_pi = nova_init(sb, sbi->initsize);
	// 	// retval = -ENOMEM;
	// 	// if (IS_ERR(root_pi)) {
	// 	// 	nova_err(sb, "%s: root_pi error.",
	// 	// 		 __func__);

	// 	// 	goto out;
	// 	// }
	// 	// goto setup_sb;
	// 	ovl_init_blockmap(sb, 0);
	// 	ovl_lite_journal_hard_init(sb);
	// } else {
	// 	ovl_rebuild_dentry_cache(sb);
	// }

	ovl_init_blockmap(sb, 0);
	ovl_lite_journal_hard_init(sb);
	// ovl_rebuild_dentry_cache(sb, sbi->virt_addr);

	

	// sbi->dram_pool = vzalloc(137438953472UL); // 128G
	// sbi->dram_pool = vzalloc(128849018880UL); // 120G
	if(sbi->enable_dram_page) {
		sbi->dram_pool = vzalloc_node(128849018880UL, 0);
		BUG_ON(!sbi->dram_pool);
		pr_info("dram_pool: %lx\n", sbi->dram_pool);



		for(i = 0; i < sbi->cpus; i++) {
			sbi->dram_alloc[i] = (128849018880UL / PAGE_SIZE / sbi->cpus) * i;
			pr_info("cpu %d: dram_alloc: %lu\n", i, sbi->dram_alloc[i]);
		}
	}



	// m = ovl_get_upper(sb, ofs, &underlying);
	// pr_info("ovl_get_upper: %d\n", m);

	// sbi->underlying_file = filp_open("/mnt/sdb/a", O_RDONLY, NULL);
	// BUG_ON(!sbi->underlying_file);
	
	// pr_info("underlying_file %lx", sbi->underlying_file);
	// if(sbi->underlying_file) {
	// 	pr_info("underlying_file name: %s", sbi->underlying_file->f_path.dentry->d_name.name);
	// }
	

	// underlying.dentry->d_inode->i_op->
	// open_with_fake_path("/mnt/sdb/a.txt")
	
	// build_open_flags(NULL, O_RDONLY, &op);
	// sbi->underlying_file = do_filp_open(99999, getname("/mnt/sdb/a.txt"), &op);
	// extern long do_sys_open(int dfd, const char __user *filename, int flags,
	// 		de_t mode);

	return 0;
out:
	return -1;
}

static struct dentry *ovl_mount(struct file_system_type *fs_type, int flags,
				const char *dev_name, void *raw_data)
{
	return mount_bdev(fs_type, flags, dev_name, raw_data, ovl_fill_super);
}

static void ovl_kill_sb(struct super_block *sb) {

	if(sb->s_root)
		d_genocide(sb->s_root);

	rcu_barrier();

	kill_block_super(sb);

	rcu_barrier();

}

static struct file_system_type ovl_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "p2cache",
	// .fs_flags	= FS_USERNS_MOUNT,
	.mount		= ovl_mount,
	// .kill_sb	= kill_block_super,
	.kill_sb	= ovl_kill_sb,
};

// MODULE_ALIAS_FS("ovl");

static void ovl_inode_init_once(void *foo)
{
	struct ovl_inode_info *oi = foo;
	inode_init_once(&oi->vfs_inode);
}

static int __init ovl_init(void)
{
	int err = 0;

	ovl_support_clwb = 1;

	pcache_inode_cachep = kmem_cache_create("pcache_inode_info",
					     sizeof(struct ovl_inode_info), 0,
					     (SLAB_RECLAIM_ACCOUNT|
					      SLAB_MEM_SPREAD),
					     ovl_inode_init_once);
	if (pcache_inode_cachep == NULL)
		return -ENOMEM;

	err = init_rangenode_cache();
	if (err)
		return err;

	// err = ovl_aio_request_cache_init();
	if (!err) {
		err = register_filesystem(&ovl_fs_type);
		if (!err)
			return 0;

		// ovl_aio_request_cache_destroy();
	}
	kmem_cache_destroy(pcache_inode_cachep);

	return err;
}


static void __exit ovl_exit(void)
{
	unregister_filesystem(&ovl_fs_type);

	/*
	 * Make sure all delayed rcu free inodes are flushed before we
	 * destroy cache.
	 */
	rcu_barrier();
	destroy_inodecache();
	destroy_rangenode_cache();
	// ovl_aio_request_cache_destroy();
}

module_init(ovl_init);
module_exit(ovl_exit);

void ovl_error_mng(struct super_block *sb, const char *fmt, ...)
{
	va_list args;

	printk(KERN_CRIT "ovl error: ");
	va_start(args, fmt);
	vprintk(fmt, args);
	va_end(args);

	// if (test_opt(sb, ERRORS_PANIC))
	// 	panic("ovl: panic from previous error\n");
	// if (test_opt(sb, ERRORS_RO)) {
	// 	printk(KERN_CRIT "ovl err: remounting filesystem read-only");
	// 	sb->s_flags |= MS_RDONLY;
	// }
}