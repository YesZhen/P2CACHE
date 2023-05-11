#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cred.h>
#include <linux/xattr.h>
#include <linux/posix_acl.h>
#include <linux/ratelimit.h>
#include <linux/fiemap.h>
// #include <linux/fileattr.h>
#include <linux/security.h>
#include <linux/namei.h>
#include "overlayfs.h"

#include <linux/pagemap.h>

static const struct inode_operations ovl_file_inode_operations = {
	.setattr	= simple_setattr,
	// .permission	= ovl_permission,
	.getattr	= simple_getattr,
	// .listxattr	= ovl_listxattr,
	// .get_acl	= ovl_get_acl,
	// .update_time	= ovl_update_time,
	// .fiemap		= ovl_fiemap,
	// .fileattr_get	= ovl_fileattr_get,
	// .fileattr_set	= ovl_fileattr_set,
};



static const struct inode_operations ovl_symlink_inode_operations = {
	.setattr	= simple_setattr,
	// .get_link	= ovl_get_link,
	.getattr	= simple_getattr,
	// .listxattr	= ovl_listxattr,
	// .update_time	= ovl_update_time,
};

static const struct inode_operations ovl_special_inode_operations = {
	// .setattr	= ovl_setattr,
	// .permission	= ovl_permission,
	// .getattr	= ovl_getattr,
	// .listxattr	= ovl_listxattr,
	// .get_acl	= ovl_get_acl,
	// .update_time	= ovl_update_time,
};

// static const struct address_space_operations ovl_aops = {
// 	/* For O_DIRECT dentry_open() checks f_mapping->a_ops->direct_IO */
// 	.direct_IO		= noop_direct_IO,
// 	// .write_begin = simple_write_begin()
// 	// .write_end = simple_write_end()
// };

static int __ovl_set_page_dirty_no_writeback(struct page *page)
{
	if (!PageDirty(page))
		return !TestSetPageDirty(page);
	return 0;
}

static const struct address_space_operations ovl_aops = {
	.readpage	= simple_readpage,
	.write_begin	= simple_write_begin,
	.write_end	= simple_write_end,
	.set_page_dirty	= __ovl_set_page_dirty_no_writeback,
};

static inline void ovl_lockdep_annotate_inode_mutex_key(struct inode *inode)
{
// #ifdef CONFIG_LOCKDEP
// 	static struct lock_class_key ovl_i_mutex_key[OVL_MAX_NESTING];
// 	static struct lock_class_key ovl_i_mutex_dir_key[OVL_MAX_NESTING];
// 	static struct lock_class_key ovl_i_lock_key[OVL_MAX_NESTING];

// 	int depth = inode->i_sb->s_stack_depth - 1;

// 	if (WARN_ON_ONCE(depth < 0 || depth >= OVL_MAX_NESTING))
// 		depth = 0;

// 	if (S_ISDIR(inode->i_mode))
// 		lockdep_set_class(&inode->i_rwsem, &ovl_i_mutex_dir_key[depth]);
// 	else
// 		lockdep_set_class(&inode->i_rwsem, &ovl_i_mutex_key[depth]);

// 	lockdep_set_class(&OVL_I(inode)->lock, &ovl_i_lock_key[depth]);
// #endif
}

static void ovl_fill_inode(struct inode *inode, umode_t mode, dev_t rdev)
{
	// inode->i_mode = mode;
    // inode->i_mode |= S_IRWXU;
	// inode->i_flags |= S_NOCMTIME;
    // inode->i_uid = {0};
    // inode->i_gid = {0};
// #ifdef CONFIG_FS_POSIX_ACL
// 	inode->i_acl = inode->i_default_acl = ACL_DONT_CACHE;
// #endif

    inode_init_owner(inode, NULL, S_IFDIR | S_IRWXU);
	ovl_lockdep_annotate_inode_mutex_key(inode);

	switch (mode & S_IFMT) {
	case S_IFREG:
		inode->i_op = &ovl_file_inode_operations;
		inode->i_fop = &ovl_file_operations;
		// inode->i_mapping->a_ops = &ovl_aops;
		break;

	case S_IFDIR:
		inode->i_op = &ovl_dir_inode_operations;
		inode->i_fop = &ovl_dir_operations;
		break;

	case S_IFLNK:
		// inode->i_op = &ovl_symlink_inode_operations;
		inode->i_op = &page_symlink_inode_operations;
		inode->i_mapping->a_ops = &ovl_aops;
		inode_nohighmem(inode);
		break;

	default:
		inode->i_op = &ovl_special_inode_operations;
		init_special_inode(inode, mode, rdev);
		break;
	}
}

struct inode *ovl_new_inode(struct super_block *sb, umode_t mode, dev_t rdev)
{
	// struct inode *inode;

	// inode = new_inode(sb);
	// if (inode)
	// 	ovl_fill_inode(inode, mode, rdev);

	// return inode;
	return NULL;

}

struct inode *pcache_get_inode(struct super_block *sb,
				const struct inode *dir, umode_t mode, dev_t dev, unsigned long ino)
{
	struct inode * inode = new_inode(sb);

	if (inode) {
		if(!ino)
			inode->i_ino = get_next_ino() + 2;
		else
			inode->i_ino = ino;

		inode_init_owner(inode, dir, mode);
		inode->i_mapping->a_ops = &ovl_aops;
		mapping_set_gfp_mask(inode->i_mapping, GFP_HIGHUSER);
		mapping_set_unevictable(inode->i_mapping);
		inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
		switch (mode & S_IFMT) {
		default:
			init_special_inode(inode, mode, dev);
			break;
		case S_IFREG:
			inode->i_op = &ovl_file_inode_operations;
			inode->i_fop = &ovl_file_operations;
			break;
		case S_IFDIR:
			inode->i_op = &ovl_dir_inode_operations;
			inode->i_fop = &ovl_dir_operations;

			/* directory inodes start off with i_nlink == 2 (for "." entry) */
			inc_nlink(inode);
			break;
		case S_IFLNK:
			inode->i_op = &page_symlink_inode_operations;
			inode_nohighmem(inode);
			break;
		}
	}
	return inode;
}

struct inode *ovl_iget(struct super_block *sb, unsigned long ino, umode_t mode)
{
	struct inode *inode;
	
	inode = iget_locked(sb, ino);

	
	if(unlikely(!inode))
		return ERR_PTR(-ENOMEM);
	if(!(inode->i_state & I_NEW)) {
		
		return inode;
	}

	// inode->i_mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP;


	// i_uid_write(inode, le32_to_cpu(0));
	// i_gid_write(inode, le32_to_cpu(0));

	// inode_init_owner()

	// if(ino == 1)
	// 	inode->i_mode |= S_IFDIR;
	// else
	// 	inode->i_mode |= S_IFREG;

	switch (mode & S_IFMT) {
	case S_IFREG:
		inode->i_mode |= S_IFREG;
		inode->i_op = &ovl_file_inode_operations;
		// if (!test_opt(inode->i_sb, DATA_COW) && wprotect == 0)
		// 	inode->i_fop = &ovl_dax_file_operations;
		// else
		// 	inode->i_fop = &ovl_wrap_file_operations;
		inode->i_fop = &ovl_file_operations;
		inode->i_size = 0;
		// inode->i_mapping
		break;
	case S_IFDIR:
		inode->i_mode |= S_IFDIR;
		inode->i_op = &ovl_dir_inode_operations;
		inode->i_fop = &ovl_dir_operations;
		set_nlink(inode, le16_to_cpu(2));
		break;
	case S_IFLNK:
		inode->i_op = &ovl_symlink_inode_operations;
		// inode->i_mapping->a_ops = &ovl_aops;
		break;
	default:
		// inode->i_op = &ovl_special_inode_operations;
		init_special_inode(inode, inode->i_mode, 0);
		break;
	}

	inode->i_flags &=
		~(S_SYNC | S_IMMUTABLE | S_NOATIME | S_DIRSYNC);

	if(ino == 1) {
		inode->i_size = 4096;
		inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	} else {
		inode->i_size = 0;
		inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	}
	

	inode->i_ino = ino;
	// pr_info("%lu\n", inode->i_count);
	// inode->i_count = 0;

	unlock_new_inode(inode);
	// pr_info("%lu\n", inode->i_count);
	return inode;

}

struct inode *ovl_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, dev_t dev)
{
	struct inode * inode = new_inode(sb);

	if (inode) {
		inode->i_ino = get_next_ino();
		mode |= S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR |
                            S_IWGRP | S_IXUSR | S_IXGRP | S_IXOTH;
		inode->i_mode = mode;
		// i_uid_write(inode, le32_to_cpu(0));
    	// i_gid_write(inode, le32_to_cpu(0));
		
		// inode->i_mapping->a_ops = &ovl_aops;
		// mapping_set_gfp_mask(inode->i_mapping, GFP_HIGHUSER);
		// mapping_set_unevictable(inode->i_mapping);
		inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
		switch (mode & S_IFMT) {
		default:
			init_special_inode(inode, mode, dev);
			break;
		case S_IFREG:
			inode->i_op = &ovl_file_inode_operations;
			inode->i_fop = &ovl_file_operations;
			break;
		case S_IFDIR:
			pr_info("dir!!!");
			inode->i_op = &ovl_dir_inode_operations;
			inode->i_fop = &ovl_dir_operations;
			inode->i_size = 4096;
			inode->i_blocks = 1;
			/* directory inodes start off with i_nlink == 2 (for "." entry) */
			inc_nlink(inode);
			break;
		case S_IFLNK:
			inode->i_op = &page_symlink_inode_operations;
			inode_nohighmem(inode);
			break;
		}
	}
	return inode;
}