#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/file.h>
#include <linux/xattr.h>
#include <linux/rbtree.h>
#include <linux/security.h>
#include <linux/cred.h>
#include <linux/ratelimit.h>
#include "overlayfs.h"

// static int ovl_dir_open()
static int times = 0;
// static int ovl_iterate(struct file *dir, struct dir_context *ctx)
// {
// 	pr_info("ovl_iterate");

// 	if(times)
// 		return 0;
// 	else
// 		times = ~times;

// 	dir_emit(ctx, "12345678", 8, 666, DT_REG);
// 	ctx->pos++;
// 	return 0;
// }

const struct file_operations ovl_dir_operations = {
	.owner		= THIS_MODULE,
	// .read		= generic_read_dir,
	.open		= dcache_dir_open,
	.release = dcache_dir_close,
    // .open       = dcache_dir_open,
	// .iterate	= ovl_iterate,
    .iterate_shared = dcache_readdir,
	// .llseek		= ovl_dir_llseek,
    // .llseek = dcache_dir_lseek,
	.fsync		= noop_fsync,
    // .fsync = noop_fsync,
	// .release	= ovl_dir_release,
    // .release = dcache_dir_close,
};