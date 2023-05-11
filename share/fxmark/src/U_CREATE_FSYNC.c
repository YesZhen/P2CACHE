// SPDX-License-Identifier: MIT
/**
 * Nanobenchmark: ADD
 *   IA. PROCESS = {create empty files and fsync at /test/$PROCESS}
 *      - TEST: inode alloc
 */	      
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "fxmark.h"
#include "util.h"

static void set_test_root(struct worker *worker, char *test_root)
{
	struct fx_opt *fx_opt = fx_opt_worker(worker);
	sprintf(test_root, "%s", fx_opt->root);
}

static int pre_work(struct worker *worker)
{
	char test_root[PATH_MAX];
	set_test_root(worker, test_root);
	return mkdir_p(test_root);
}

static int main_work(struct worker *worker)
{
	char test_root[PATH_MAX];
	struct bench *bench = worker->bench;
	uint64_t iter;
	int rc = 0;
    int dir_fd;

	set_test_root(worker, test_root);

    if ((dir_fd = open(test_root, O_DIRECTORY, 0)) == -1)
        goto err_out;

	for (iter = 0; !bench->stop; ++iter) {
		char file[PATH_MAX];
		int fd;
		/* create and close */
		snprintf(file, PATH_MAX, "u_create_fsync-%" PRIu64 ".dat", iter);
		if ((fd = openat(dir_fd, file, O_CREAT | O_RDWR, S_IRWXU)) == -1)
			goto err_out;
        fsync(fd);
        fsync(dir_fd);
		close(fd);
	}
    close(dir_fd);
out:
	worker->works = (double)iter;
	return rc;
err_out:
	bench->stop = 1;
	rc = errno;
	goto out;
}

struct bench_operations u_create_fsync_ops = {
	.pre_work  = pre_work, 
	.main_work = main_work,
};
