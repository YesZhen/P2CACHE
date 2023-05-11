#ifndef __STATS_H__
#define __STATS_H__


enum stats_category {
	dram_read,
    dram_write,
    pmem_read,
    pmem_write,

	/* Sentinel */
	STATS_NUM,
};

extern u64 IOstats[STATS_NUM];
DECLARE_PER_CPU(u64[STATS_NUM], IOstats_percpu);

// void ovl_clear_IO_stats(struct super_block*);

void ovl_get_IO_stats(void);

#define OVL_STATS_ADD(name, value) \
	{__this_cpu_add(IOstats_percpu[name], value); }

#endif // __STATS_H__