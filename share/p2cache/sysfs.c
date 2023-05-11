// #include "inode.h"
#include "overlayfs.h"
// #include "ovl_entry.h"
// #include "stats.h"

const char *proc_dirname = "fs/ovl";
struct proc_dir_entry *ovl_proc_root;

static int ovl_seq_IO_show(struct seq_file *seq, void *v)
{
    struct super_block *sb = seq->private;
    struct ovl_sb_info *sbi = OVL_SB(sb);

    // unsigned long dram_read = 0;
    // unsigned long dram_write = 0;
    // unsigned long pmem_read = 0;
    // unsigned long pmem_write = 0;

    ovl_get_IO_stats();

    seq_puts(seq, "================ ovl I/O stats ================\n\n");

    seq_printf(seq, "dram_read %llu, dram_write %llu, pmem_read %llu, pmem_write %llu\n",
                    IOstats[dram_read], IOstats[dram_write], IOstats[pmem_read], IOstats[pmem_write]);

    seq_puts(seq, "\n");
    return 0;

}

static int ovl_seq_IO_open(struct inode *inode, struct file *file)
{
	return single_open(file, ovl_seq_IO_show, PDE_DATA(inode));
}

ssize_t ovl_seq_clear_stats(struct file *filp, const char __user *buf,
	size_t len, loff_t *ppos)
{
	struct address_space *mapping = filp->f_mapping;
	struct inode *inode = mapping->host;
	struct super_block *sb = PDE_DATA(inode);

	ovl_clear_stats(sb);
	return len;
}

static const struct file_operations ovl_seq_IO_fops = {
    .owner  = THIS_MODULE,
    .open   = ovl_seq_IO_open,
    .read   = seq_read,
    .write  = ovl_seq_clear_stats,
    .llseek = seq_lseek,
    .release= single_release,
};

void ovl_sysfs_init(struct super_block *sb)
{
    struct ovl_sb_info *sbi = OVL_SB(sb);

    if(ovl_proc_root)
        sbi->s_proc = proc_mkdir("aaa", ovl_proc_root);
    
    if(sbi->s_proc) {
        proc_create_data("IO_stats", 0444, sbi->s_proc, &ovl_seq_IO_fops, sb);
    }
}