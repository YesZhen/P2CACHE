#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/xattr.h>
#include <linux/security.h>
#include <linux/cred.h>
#include <linux/module.h>
#include <linux/posix_acl.h>
#include <linux/posix_acl_xattr.h>
#include <linux/atomic.h>
#include <linux/ratelimit.h>
#include "overlayfs.h"

#include <linux/dcache.h>

static inline void busy_loop(void)
{
	unsigned long tick;
	for(tick = 0; tick <= 1000000; tick++)
	{
		tick++;
	}
}

static int ovl_append_log_entry(struct super_block *sb, u8 log_type, unsigned long p_ino, unsigned long c_ino, u8 name_len, char *name)
{
	struct journal_ptr_pair *pair;
	size_t size = sizeof(struct ovl_dir_log_entry);
	struct ovl_dir_log_entry *entry;

	// return 0;

	pair = ovl_get_journal_pointers(sb, smp_processor_id());
	entry = ovl_get_block(sb, pair->journal_tail);

	// memset(entry, 0, size);
	entry->c_ino = c_ino;
	entry->p_ino = p_ino;
	entry->entry_type = log_type;
	entry->name_len = name_len;
	if(name_len > 0)
		memcpy(entry->name, name, name_len + 1);

	ovl_flush_buffer(entry, size, 0);

	pair->journal_tail += size;

	PERSISTENT_BARRIER();

	ovl_flush_buffer(pair, CACHELINE_SIZE, 1);
	return 0;
}

static int ovl_inode_by_name(struct inode *dir, const struct qstr *child, ino_t *ino)
{
	// pr_info("%s", __func__);
	return 0;
}

static struct dentry *ovl_lookup(struct inode *dir, struct dentry *dentry,
				   unsigned int flags)
{
	// pr_info("%s", __func__);
	if (dentry->d_name.len > NAME_MAX)
		return ERR_PTR(-ENAMETOOLONG);
	if (!dentry->d_sb->s_d_op)
		d_set_d_op(dentry, &simple_dentry_operations);
	d_add(dentry, NULL);
	return NULL;
}

static int
ovl_mknod(struct inode *dir,
	    struct dentry *dentry, umode_t mode, dev_t dev)
{
	uint64_t begin, end;
	struct inode *inode;

	// pr_info("%s", __func__);
	

	// inode = ovl_iget(dir->i_sb, get_next_ino(), mode & S_IFMT);
	inode = pcache_get_inode(dir->i_sb, dir, mode, dev, 0);
	int error = -ENOSPC;

	
	if (inode) {		

		d_instantiate(dentry, inode);
		dget(dentry);	/* Extra count - pin the dentry in core */
		// pr_info("%lu\n", dentry->d_lockref.count);
		error = 0;
		dir->i_mtime = dir->i_ctime = current_time(dir);
	}

	ovl_append_log_entry(dir->i_sb, ((mode & S_IFMT) & S_IFDIR) ? 2 : 3, dir->i_ino, inode->i_ino, dentry->d_name.len, dentry->d_name.name);
	// busy_loop();
	return error;
}

static int ovl_mkdir(struct inode *dir,
		       struct dentry *dentry, umode_t mode)
{
	int retval; 
	// pr_info("%s", __func__);
	retval = ovl_mknod(dir, dentry, mode | S_IFDIR, 0);
	if (!retval)
		inc_nlink(dir);
	// busy_loop();
	return retval;
}

static int ovl_create(struct inode *dir,
			struct dentry *dentry, umode_t mode, bool excl)
{
	// pr_info("%s", __func__);
	return ovl_mknod(dir, dentry, mode | S_IFREG, 0);
}

// static int ovl_create (struct user_namespace * mnt_userns,
// 			struct inode * dir, struct dentry * dentry,
// 			umode_t mode, bool excl)
// {
// 	struct inode *inode;
// 	int error = -ENOSPC;
// 	inode = ovl_iget(dir->i_sb, get_next_ino());
	
// 	// pr_info("%s: dir: %lu, inode: %lu, %s\n", __func__, dir->i_ino, inode->i_ino, dentry->d_name.name);
	

// 	if(inode) {
// 		d_instantiate(dentry, inode);
// 		dget(dentry);
// 		error = 0;
// 		dir->i_mtime = dir->i_ctime = current_time(dir);
// 	}
// 	return 0;
// }

int ovl_link(struct dentry *old_dentry, struct inode *dir, struct dentry *dentry)
{
	int rc;
	// pr_info("%s", __func__);
	rc = simple_link(old_dentry, dir, dentry);
	ovl_append_log_entry(dir->i_sb, 6, dir->i_ino, old_dentry->d_inode->i_ino, dentry->d_name.len, dentry->d_name.name);
	// busy_loop();
	return rc;
}

int ovl_unlink(struct inode *dir, struct dentry *dentry)
{
	int rc;
	// pr_info("%s", __func__);
	rc = simple_unlink(dir, dentry);
	ovl_append_log_entry(dir->i_sb, 4,	dir->i_ino, dentry->d_inode->i_ino, NULL, NULL);
	// busy_loop();
	return rc;
}

int ovl_rename(struct inode *old_dir, struct dentry *old_dentry,
		  struct inode *new_dir, struct dentry *new_dentry,
		  unsigned int flags)
{
	int rc;
	// pr_info("%s", __func__);
	rc = simple_rename(old_dir, old_dentry, new_dir, new_dentry, flags);
	ovl_append_log_entry(old_dir->i_sb, 7, new_dir->i_ino, old_dentry->d_inode->i_ino, new_dentry->d_name.len, new_dentry->d_name.name);
	// busy_loop();
	return rc;
}

int ovl_rmdir(struct inode *dir, struct dentry *dentry)
{
	int ret;
	// pr_info("%s", __func__);
	ret = simple_rmdir(dir, dentry);
	ovl_append_log_entry(dir->i_sb, 5,	dir->i_ino, dentry->d_inode->i_ino, NULL, NULL);
	// busy_loop();
	return ret;
}

int ovl_setattr(struct dentry *dentry, struct iattr *iattr)
{
	int ret;
	// pr_info("%s", __func__);
	ret = simple_setattr(dentry, iattr);
	// busy_loop();
	return ret;
}

static int ovl_symlink(struct inode * dir, struct dentry *dentry, const char * symname)
{
	// struct inode *inode;
	// struct super_block *sb;
	// int error = -ENOSPC;
	// unsigned long num_blocks_need, blocknr;
	// int allocated;
	// void *kmem;
	// struct ovl_inode_info_header *sih;

	// sb = dir->i_sb;

	// // inode = ovl_iget(dir->i_sb, dir, S_IFLNK|S_IRWXUGO, 0);
	// inode = ovl_iget(dir->i_sb, get_next_ino(), S_IFLNK|S_IRWXUGO);
	// if (inode) {

	// 	sih = OVL_IH(inode);

	// 	num_blocks_need = 1;
	// 	allocated = ovl_new_data_blocks(sb, NULL, &blocknr, 0,
	// 					num_blocks_need, ALLOC_INIT_ZERO, ANY_CPU,
	// 					ALLOC_FROM_HEAD);

	// 	BUG_ON(allocated != num_blocks_need);

	// 	kmem = ovl_get_block(sb, ovl_get_block_off(sb, blocknr, 0));

	// 	int l = strlen(symname)+1;

	// 	error = memcpy_to_pmem_nocache(kmem, symname, l);
	// 	// error = page_symlink(inode, symname, l);
	// 	if (!error) {
	// 		d_instantiate(dentry, inode);
	// 		dget(dentry);
	// 		dir->i_mtime = dir->i_ctime = current_time(dir);
	// 		ovl_store_page_addr(&sih->pmem_pages, 0, kmem);

	// 	} else {
	// 		iput(inode);
	// 		BUG();
	// 	}
	// }
	// return error;
	struct inode *inode;
	int error = -ENOSPC;

	inode = pcache_get_inode(dir->i_sb, dir, S_IFLNK|S_IRWXUGO, 0, 0);
	if (inode) {
		int l = strlen(symname)+1;
		error = page_symlink(inode, symname, l);
		if (!error) {
			d_instantiate(dentry, inode);
			dget(dentry);
			dir->i_mtime = dir->i_ctime = current_time(dir);
		} else
			iput(inode);
	}
	return error;
}


const struct inode_operations ovl_dir_inode_operations = {
	// .lookup		= ovl_lookup,
	.lookup = simple_lookup,
	.mkdir		= ovl_mkdir,
	.symlink	= ovl_symlink,
	.unlink		= ovl_unlink,
	.rmdir		= ovl_rmdir,
	.rename		= ovl_rename,
	.link		= ovl_link,
	.setattr	= ovl_setattr,
	.create		= ovl_create,
	// .create = simple_create,

	.mknod		= ovl_mknod,
	// .permission	= ovl_permission,
	.getattr	= simple_getattr,
	// .listxattr	= ovl_listxattr,
	// .get_acl	= ovl_get_acl,
	// .update_time	= ovl_update_time,
	// .fileattr_get	= ovl_fileattr_get,
	// .fileattr_set	= ovl_fileattr_set,
};

// mkdir
// unlink
// rmdir
// rename
// create
