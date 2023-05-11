/* SPDX-License-Identifier: MIT */
#ifndef __FX_H__
#define __FX_H__
#include <linux/limits.h>
#include "bench.h"

#define FX_OPT_MAX_PRIVATE 4

#define PCACHE_ACCESS_SIZE 1024UL
#define FILE_SIZE 4294967296UL
// #define FILE_SIZE 8589934592UL

struct fx_opt {
	char root[PATH_MAX];
	uint64_t private[FX_OPT_MAX_PRIVATE];
};

#define fx_opt_bench(__b) ((struct fx_opt *)((__b)->args))
#define fx_opt_worker(__w)  fx_opt_bench((__w)->bench)

struct cmd_opt {
	struct bench_operations *ops;
	int ncore;
	int nbg;
	int duration;
	int directio;
	char *root;
	char *profile_start_cmd;
	char *profile_stop_cmd;
	char *profile_stat_file;
	unsigned long d_as;
	unsigned long d_ss;
	unsigned long d_ts;
	unsigned long d_p_as;
	unsigned long d_p_ss;
	unsigned long d_p_ts;
	int sync;
	int op;
	unsigned long m_amount;
};

/* benchmarks */ 
extern struct bench_operations n_inode_alloc_ops;
extern struct bench_operations n_blk_alloc_ops;
extern struct bench_operations n_blk_wrt_ops;
extern struct bench_operations n_dir_ins_ops;
extern struct bench_operations n_jnl_cmt_ops;
extern struct bench_operations n_mtime_upt_ops;
extern struct bench_operations n_file_rename_ops;
extern struct bench_operations n_file_rd_ops;
extern struct bench_operations n_file_rd_bg_ops;
extern struct bench_operations n_shfile_rd_ops;
extern struct bench_operations n_shfile_rd_bg_ops;
extern struct bench_operations n_shblk_rd_ops;
extern struct bench_operations n_shblk_rd_bg_ops;
extern struct bench_operations n_dir_rd_ops;
extern struct bench_operations n_dir_rd_bg_ops;
extern struct bench_operations n_shdir_rd_ops;
extern struct bench_operations n_shdir_rd_bg_ops;
extern struct bench_operations n_priv_path_rsl_ops;
extern struct bench_operations n_path_rsl_ops;
extern struct bench_operations n_path_rsl_bg_ops;
extern struct bench_operations n_spath_rsl_ops;
extern struct bench_operations u_file_cr_ops;
extern struct bench_operations u_file_rm_ops;
extern struct bench_operations u_sh_file_rm_ops;
extern struct bench_operations u_file_tr_ops;
extern struct bench_operations n_create_fsync_ops;
extern struct bench_operations n_create_ops;
extern struct bench_operations u_create_fsync_ops;
extern struct bench_operations n_mkdir_fsync_ops;
extern struct bench_operations n_rename_fsync_ops;
extern struct bench_operations n_unlink_fsync_ops;
extern struct bench_operations n_link_fsync_ops;
extern struct bench_operations n_ow_1k_fdatasync_ops;
extern struct bench_operations n_aw_1k_fdatasync_ops;
extern struct bench_operations n_or_1k_ops;
extern struct bench_operations n_ar_1k_ops;
extern struct bench_operations n_aw_4k_fdatasync_ops;
extern struct bench_operations n_ar_4k_ops;
extern struct bench_operations data_ops;
extern struct bench_operations meta_ops;

#endif /* __FX_H__ */
