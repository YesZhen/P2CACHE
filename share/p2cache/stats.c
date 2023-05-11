#include "overlayfs.h"


u64 IOstats[STATS_NUM];
DEFINE_PER_CPU(u64[STATS_NUM], IOstats_percpu);

static void ovl_clear_IO_stats(struct super_block *sb)
{
	struct ovl_sb_info *sbi = OVL_SB(sb);
	struct free_list *free_list;
	int i;
	int cpu;

	for (i = 0; i < STATS_NUM; i++) {
		IOstats[i] = 0;
		for_each_possible_cpu(cpu)
			per_cpu(IOstats_percpu[i], cpu) = 0;
	}

	// for (i = 0; i < sbi->cpus; i++) {
	// 	free_list = nova_get_free_list(sb, i);

	// 	free_list->alloc_log_count = 0;
	// 	free_list->alloc_log_pages = 0;
	// 	free_list->alloc_data_count = 0;
	// 	free_list->alloc_data_pages = 0;
	// 	free_list->free_log_count = 0;
	// 	free_list->freed_log_pages = 0;
	// 	free_list->free_data_count = 0;
	// 	free_list->freed_data_pages = 0;
	// }
}

void ovl_clear_stats(struct super_block *sb)
{
	// ovl_clear_timing_stats();
	ovl_clear_IO_stats(sb);
}

void ovl_get_IO_stats(void)
{
    int i;
    int cpu;
    for(i = 0; i < STATS_NUM; i++) {
        IOstats[i] = 0;
        for_each_possible_cpu(cpu)
            IOstats[i] += per_cpu(IOstats_percpu[i], cpu);
    }
}