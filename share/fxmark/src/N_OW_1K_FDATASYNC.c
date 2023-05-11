// SPDX-License-Identifier: MIT
/**
 * Nanobenchmark: block write
 *   BW. PROCESS = {ovewrite file at /test/$PROCESS}
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

// #define FILE_SIZE 8589934592UL
// #define ONE_GB 1073741824UL

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

    size_t len = 32768;

    // printf("aaaa\n");

	/* create test root */
	set_test_root(worker, test_root);
	rc = mkdir_p(test_root);
	if (rc) return rc;

	/* create a test file */ 
	snprintf(file, PATH_MAX, "%s/n_ow_1k_fdatasync.dat", test_root);
	if ((fd = open(file, O_CREAT | O_RDWR, S_IRWXU)) == -1)
		goto err_out;

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

	// if (write(fd, page, PAGE_SIZE) != PAGE_SIZE)
	//   goto err_out;
    
    for(loff_t pos = 0; pos + len <= FILE_SIZE; pos += len) {
        if(pwrite(fd, page, len, pos) != len)
            goto err_out;
    }
	// if(pwrite(fd, page, 4096, ONE_GB) != 4096)
	// 	goto err_out;

    fsync(fd);
out:
	/* put fd to worker's private */
	worker->private[0] = (uint64_t)fd;
	return rc;
err_out:
    printf("pre_work error, %d\n", errno);
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

#if DEBUG 
	fprintf(stderr, "DEBUG: worker->id[%d], main worker address :%p\n",
		worker->id, worker->page);
#endif
	assert(page);

	/* fsync */
	fd = (int)worker->private[0];
    loff_t pos = 0;
    size_t len = PCACHE_ACCESS_SIZE;
	for (iter = 0; !bench->stop && pos + len <= FILE_SIZE; ++iter, pos += len) {
	  if (pwrite(fd, page, len, pos) != len)
		goto err_out;
      fdatasync(fd);
	}
out:
	close(fd);
	worker->works = (double)iter;
	return rc;
err_out:
	bench->stop = 1;
	rc = errno;
    printf("main work error, %d\n", rc);
    free(page);
	goto out;
}

struct bench_operations n_ow_1k_fdatasync_ops = {
	.pre_work  = pre_work, 
	.main_work = main_work,
};
