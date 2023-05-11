// SPDX-License-Identifier: MIT
/**
 * Nanobenchmark: data write
 *   BW. PROCESS = {write file at /test/$PROCESS}
 *       - TEST: ideal, no conention
 */	      
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include "fxmark.h"
#include "util.h"


static void set_test_root(struct worker *worker, char *test_root)
{
	struct fx_opt *fx_opt = fx_opt_worker(worker);
	sprintf(test_root, "%s/%d", fx_opt->root, worker->id);
}

static int pre_work(struct worker *worker)
{
    char *page = NULL;
    struct bench *bench = worker->bench;
	char test_root[PATH_MAX];
	char file[PATH_MAX];
	int fd, rc = 0;
	struct cmd_opt *opt = bench->opt;

    size_t len, step, file_size;

	// fprintf(stderr, "%lu, %lu, %lu, %lu, %lu, %lu, %d, %d\n", opt->d_p_as, opt->d_p_ss, opt->d_p_ts, opt->d_as, opt->d_ss, opt->d_ts, opt->sync, opt->op);

	/* create test root */
	set_test_root(worker, test_root);
	rc = mkdir_p(test_root);
	if (rc)
		goto err_out;

	/* create a test file */ 
	snprintf(file, PATH_MAX, "%s/data_bench.dat", test_root);
	if ((fd = open(file, O_CREAT | O_RDWR, S_IRWXU)) == -1)
		goto err_out;

	len = 1048576;
	if(posix_memalign((void **)&(worker->page), PAGE_SIZE, len))
	  goto err_out;
	page = worker->page;
	if (!page)
		goto err_out;

#if DEBUG
	/*to debug*/
	fprintf(stderr, "DEBUG: worker->id[%d], page address :%p\n",worker->id, page);
#endif

	/*set flag with O_DIRECT if necessary*/
	if(bench->directio && (fcntl(fd, F_SETFL, O_DIRECT)==-1))
		goto err_out;

	if(!opt->d_p_ts || !opt->d_p_as)
		goto out;
	

	// if (write(fd, page, PAGE_SIZE) != PAGE_SIZE)
	//   goto err_out;
    len = opt->d_p_as;
	step = opt->d_p_ss;
	file_size = opt->d_p_ts;
    for(loff_t pos = 0; pos + len <= file_size; pos += step) {
        if(pwrite(fd, page, len, pos) != len)
            goto err_out;
    }

	// fsync(fd);
    
out:
	
	/* put fd to worker's private */
	worker->private[0] = (uint64_t)fd;
	return rc;
err_out:
	bench->stop = 1;
	rc = errno;
	free(page);
	goto out;
}

static int main_work(struct worker *worker)
{
  	char *page = worker->page;
	struct bench *bench = worker->bench;
	int fd, rc = 0;
	uint64_t iter = 0;
	struct cmd_opt *opt = bench->opt;

#if DEBUG 
	fprintf(stderr, "DEBUG: worker->id[%d], main worker address :%p\n",
		worker->id, worker->page);
#endif
	assert(page);

	/* fsync */
	fd = (int)worker->private[0];
    loff_t pos = 0;
    size_t len = opt->d_as;
	size_t step = opt->d_ss;
	size_t file_size = opt->d_ts;
	int op = opt->op;
	int sync = opt->sync;

	ssize_t (*rw_func)(int, const void *, size_t, off_t);
	// ssize_t pwrite(int __fd, const void *__buf, size_t __n, off_t __offset)

	rw_func = op ? &pwrite : &pread;

	for (iter = 0; !bench->stop; ++iter, pos += step) {
		
		if(pos + len > file_size)
			if(op)
				goto out;
			else
				goto out;
	
		if (rw_func(fd, page, len, pos) != len)
			goto err_out;
	
		if (sync)
    		fdatasync(fd);
	}
out:
	close(fd);
	worker->works = (double)iter;
	return rc;
err_out:
	bench->stop = 1;
	rc = errno;
    free(page);
	goto out;
}

struct bench_operations data_ops = {
	.pre_work  = pre_work, 
	.main_work = main_work,
};
