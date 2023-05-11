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

// op: 2)create, 3)mkdir, 4)unlink, 5)rmdir, 6)link, 7)rename

static int pre_work(struct worker *worker)
{
    struct bench *bench = worker->bench;
	char test_root[PATH_MAX];
	char file[PATH_MAX];
	int op, dir_fd, fd, rc = 0;
    unsigned long m_amount;
	struct cmd_opt *opt = bench->opt;

	// fprintf(stderr, "%lu, %d, %d\n", opt->m_amount, opt->sync, opt->op);

	/* create test root */
	set_test_root(worker, test_root);
	rc = mkdir_p(test_root);
	if (rc) 
        goto err_out;

    /* time to create files */

    if ((dir_fd = open(test_root, O_DIRECTORY , 0)) < 0)
        goto err_out;

    op = opt->op;
    m_amount = opt->m_amount;
    if(op >=4 && op <= 7) {
        for (worker->private[0] = 0;worker->private[0] < m_amount; ++worker->private[0]) {
            sprintf(file, "%s/meta_bench_%lu", test_root, worker->private[0]);

            if(op == 5) {
                fd = mkdir(file, S_IRWXU); // mkdir return 0 on success
            } else {
                fd = creat(file, S_IRWXU); // create a file
            }
            if (fd == -1) {
                if (errno == ENOSPC) {
                    --worker->private[0];
                    rc = 0;
                    goto out;
                }
                rc = errno;
                goto err_out;
            }
            // if(op != 5) // not mkdir
            //     close(fd);
        }
    } else {
        worker->private[0] = m_amount;
    }
    // fsync(dir_fd);
    close(dir_fd);

#if DEBUG
	/*to debug*/
	fprintf(stderr, "DEBUG: worker->id[%d], page address :%p\n",worker->id, page);
#endif
    
out:
	return rc;
err_out:
    fprintf(stderr, "prework err_out %d\n", errno);
	bench->stop = 1;
	rc = errno;
	goto out;
}

static int main_work(struct worker *worker)
{
	uint64_t iter = 0;
    struct bench *bench = worker->bench;
	char test_root[PATH_MAX];
	char file[PATH_MAX];
    char new_file[PATH_MAX];
	int sync, op, dir_fd, fd, rc = 0;
    unsigned long m_amount;
	struct cmd_opt *opt = bench->opt;
	

#if DEBUG 
	fprintf(stderr, "DEBUG: worker->id[%d], main worker address :%p\n",
		worker->id, worker->page);
#endif

    // fprintf(stderr, "%lu, %d, %d\n", worker->private[0], opt->sync, opt->op);

    set_test_root(worker, test_root);
    m_amount = opt->m_amount;
    op = opt->op;
    sync = opt->sync;

    for (iter = 0; iter < worker->private[0] && !bench->stop; ++iter) {
        sprintf(file, "%s/meta_bench_%lu", test_root, iter);

        if(op == 2) {
            fd = creat(file, S_IRWXU);
        } else if (op == 3) {
            fd = mkdir(file, S_IRWXU);
        } else if (op == 4) {
            fd = unlink(file);
        } else if (op == 5) {
            fd = rmdir(file);
        } else if (op == 6) {
            sprintf(new_file, "%s/new_meta_bench_%lu", test_root, iter);
            fd = link(file, new_file);
        } else if (op == 7) {
            sprintf(new_file, "%s/new_meta_bench_%lu", test_root, iter);
            fd = rename(file, new_file);
        }
        
        if(fd == -1)
            goto err_out;
        
        if(sync) {
            if(op == 2) {
                fd = open(file, O_RDWR, NULL);
                if(fsync(fd) == -1)
                    goto err_out;
            } else if (op == 3) {
                fd = open(file, O_DIRECTORY, NULL);
                if(fsync(fd) == -1)
                    goto err_out;
            } else if(op == 6 || op == 7) {
                fd = open(new_file, O_RDWR, NULL);
                if(fsync(fd) == -1)
                    goto err_out;
            }
            close(fd);
            fsync(dir_fd);
        }
    }
out:
	// close(dir_fd);
	worker->works = (double)iter;
	return rc;
err_out:
    fprintf(stderr, "mainwork err_out %d, %lu\n", errno, iter);
	bench->stop = 1;
	rc = errno;
	goto out;
}

struct bench_operations meta_ops = {
	.pre_work  = pre_work, 
	.main_work = main_work,
};
