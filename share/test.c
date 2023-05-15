#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
// #define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>


int mkdir_p(const char *path)
{
	char cmd[PATH_MAX*2];
	snprintf(cmd, PATH_MAX*2, "mkdir -p %s", path);
	return system(cmd);
}

void do_write(int fd, char* write_buf, size_t sz, off_t offset, char* fake_file) {
    int rc;
    rc = pwrite(fd, write_buf, sz, offset);
    
    if(rc != sz) {
        printf("write failed, rc=%lu, sz=%lu", rc, sz);
        exit(1);
    }

    for(size_t i = 0; i < sz; i++) {
        fake_file[offset + i] = write_buf[i];
    }
}

void do_read(int fd, char* read_buf, size_t sz, off_t offset, char* fake_file) {
    int rc;

    rc = pread(fd, read_buf, sz, offset);
    if(rc != sz) {
        printf("read failed, rc=%lu, sz=%lu", rc, sz);
        exit(1);
    }

    for(size_t i = 0; i < sz; i++) {
        if(fake_file[offset + i] != read_buf[i]) {
            printf("read not correct, pos=%lu\n", offset + i);
            exit(1);
        }
    }
}

void zero_buf(char* buf, size_t sz) {
    memset(buf, 0, sz);
}

// #define FILE_SIZE 134217728
#define FILE_SIZE 1048576
// #
#define ONE_GB 1073741824UL
// #define FILE_SIZE 4194304
#define MAX_WRITE_SIZE 1048576

void do_append_test() {

    char test_root[PATH_MAX], file[PATH_MAX];
    int rc, fd, dir_fd;
    loff_t pos;
    size_t len;

    char write_buf[MAX_WRITE_SIZE];
    // char read_buf[FILE_SIZE];
    // char fake_file[FILE_SIZE];

    char* read_buf = malloc(FILE_SIZE);
    char* fake_file = malloc(FILE_SIZE); 

    struct stat st;


    sprintf(test_root, "/mnt/data");

    for(int i = 0; i < MAX_WRITE_SIZE; i++)
        write_buf[i] = i % 256;
    
    zero_buf(read_buf, FILE_SIZE);
    zero_buf(fake_file, FILE_SIZE);

    if ((dir_fd = open(test_root, O_DIRECTORY, 0)) == -1) {
        printf("error at open dir, rc = %d, errno = %d\n", dir_fd, errno);
        goto err_out;
    }

    for(len = 4096; len <= 4096; len++) {
        // if(len % 1)
        printf("testing len %lu\n", len);
        // if(len % 256 == 0)
        //     printf("%lu ", len);

        sprintf(file, "append-%lu.dat", len);
        if ((fd = openat(dir_fd, file,  O_CREAT | O_RDWR, S_IRWXU)) == -1) {
            printf("error at open file, rc = %d, errno = %d\n", fd, errno);
            goto err_out;
        }

        zero_buf(fake_file, FILE_SIZE);

        for(pos = 0; pos + len <= FILE_SIZE; pos += len) {
            do_write(fd, write_buf, len, pos, fake_file);
            sprintf(file, "/mnt/data/append-%lu.dat", len);
            stat(file, &st);
            if(st.st_size != pos + len) {
                printf("size not correct, %lu, %lu\n", st.st_size, pos + len);
                goto err_out;
            }

            do_read(fd, read_buf, len, pos, fake_file);
        }

        do_read(fd, read_buf, pos, 0, fake_file);

        close(fd);
        // printf("\n");
    }

    close(dir_fd);

    return;

err_out:
    printf("seq err_out\n");
    exit(1);
    return;

}

void do_overwrite_test() {

    char test_root[PATH_MAX], file[PATH_MAX];
    int rc, fd, dir_fd;
    loff_t pos;
    size_t len;

    char write_buf[MAX_WRITE_SIZE];
    // char read_buf[FILE_SIZE];
    // char fake_file[FILE_SIZE];
    char* read_buf = malloc(FILE_SIZE);
    char* fake_file = malloc(FILE_SIZE); 


    struct stat st;

    sprintf(test_root, "/mnt/data");

    for(int i = 0; i < MAX_WRITE_SIZE; i++)
        write_buf[i] = i % 256;
    
    

    if ((dir_fd = open(test_root, O_DIRECTORY, 0)) == -1) {
        printf("error at open dir, rc = %d, errno = %d\n", dir_fd, errno);
        goto err_out;
    }

    for(len = 1; len <= 16384; len++) {

        sprintf(file, "overwrite-%lu.dat", len);
        if ((fd = openat(dir_fd, file,  O_CREAT | O_RDWR, S_IRWXU)) == -1) {
            printf("error at open file, rc = %d, errno = %d\n", fd, errno);
            goto err_out;
        }
        zero_buf(read_buf, FILE_SIZE);
        zero_buf(fake_file, FILE_SIZE);

        for(pos = 0; pos + MAX_WRITE_SIZE <= FILE_SIZE; pos += MAX_WRITE_SIZE) {
            do_write(fd, write_buf, MAX_WRITE_SIZE, pos, fake_file);
        }

        
        printf("testing len %lu\n", len);

        for(pos = 0; pos + len <= FILE_SIZE; pos += len + 119) {
            do_write(fd, write_buf, len, pos, fake_file);
            // printf("%lu ", pos);
            sprintf(file, "/mnt/data/overwrite-%lu.dat", len);
            stat(file, &st);
            if(st.st_size != FILE_SIZE) {
                printf("size not correct, %lu, %lu\n", st.st_size, FILE_SIZE);
                goto err_out;
            }
            do_read(fd, read_buf, len, pos, fake_file);
        }
        // printf("\n");
        // fake_file[5] = 0; 
        do_read(fd, read_buf, FILE_SIZE - 1, 0, fake_file);
        close(fd);
    }

    close(dir_fd);

    return;

err_out:
    printf("overwrite err_out\n");
    exit(1);
    return;

}

void do_overwrite_test1() {
    char test_root[PATH_MAX], file[PATH_MAX];
    int rc, fd, dir_fd;
    loff_t pos;
    size_t len;

    char write_buf[MAX_WRITE_SIZE];
    // char read_buf[FILE_SIZE];
    // char fake_file[FILE_SIZE];
    char* read_buf = malloc(FILE_SIZE);
    char* fake_file = malloc(FILE_SIZE); 


    struct stat st;

    sprintf(test_root, "/mnt/data");

    for(int i = 0; i < MAX_WRITE_SIZE; i++)
        write_buf[i] = i % 256;
    
    

    if ((dir_fd = open(test_root, O_DIRECTORY, 0)) == -1) {
        printf("error at open dir, rc = %d, errno = %d\n", dir_fd, errno);
        goto err_out;
    }

    sprintf(file, "overwrite1.dat");
    if ((fd = openat(dir_fd, file,  O_CREAT | O_RDWR, S_IRWXU)) == -1) {
        printf("error at open file, rc = %d, errno = %d\n", fd, errno);
        goto err_out;
    }

    zero_buf(read_buf, FILE_SIZE);
    zero_buf(fake_file, FILE_SIZE);

    do_write(fd, write_buf, 4096, 0, fake_file);
    do_read(fd, read_buf, 4096, 0, fake_file);

    do_write(fd, write_buf, 2333, 872, fake_file);
    do_read(fd, read_buf, 2333, 872, fake_file);

    do_write(fd, write_buf, 913, 421, fake_file);
    do_read(fd, read_buf, 913, 421, fake_file);

    do_write(fd, write_buf, 1183, 1173, fake_file);
    do_read(fd, read_buf, 1183, 1173, fake_file);

    do_write(fd, write_buf, 333, 926, fake_file);
    do_read(fd, read_buf, 333, 926, fake_file);

    do_read(fd, read_buf, 4096, 0, fake_file);

    return;

err_out:
    return;
}

void do_hole_test() {
    char test_root[PATH_MAX], file[PATH_MAX];
    int rc, fd, dir_fd;
    loff_t pos;
    size_t len, sz;

    char write_buf[MAX_WRITE_SIZE];
    char* read_buf = malloc(FILE_SIZE);
    char* fake_file = malloc(FILE_SIZE); 

    struct stat st;

    sprintf(test_root, "/mnt/data");

    for(int i = 0; i < MAX_WRITE_SIZE; i++)
        write_buf[i] = i % 256;
    
    

    if ((dir_fd = open(test_root, O_DIRECTORY, 0)) == -1) {
        printf("error at open dir, rc = %d, errno = %d\n", dir_fd, errno);
        goto err_out;
    }

    for(len = 1; len <= 8192; len++) {
        sz = 0;

        sprintf(file, "hole-%lu.dat", len);
        if ((fd = openat(dir_fd, file,  O_CREAT | O_RDWR, S_IRWXU)) == -1) {
            printf("error at open file, rc = %d, errno = %d\n", fd, errno);
            goto err_out;
        }
        zero_buf(read_buf, FILE_SIZE);
        zero_buf(fake_file, FILE_SIZE);
        printf("testing len %lu\n", len);
        for(pos = len; pos + len <= FILE_SIZE; pos += 32768) {
            do_write(fd, write_buf, len, pos, fake_file);
            sprintf(file, "/mnt/data/hole-%lu.dat", len);
            stat(file, &st);
            if(st.st_size != pos + len) {
                printf("size not correct, %lu, %lu\n", st.st_size, pos + len);
                goto err_out;
            }
            sz = pos + len;
            do_read(fd, read_buf, len, pos, fake_file);
        }

        for(pos = len + 4096; pos + len <= FILE_SIZE; pos += 32768) {
            do_write(fd, write_buf, len, pos, fake_file);
            sz = pos + len > sz ? pos + len : sz;
            sprintf(file, "/mnt/data/hole-%lu.dat", len);
            stat(file, &st);
            if(st.st_size != sz) {
                printf("size not correct, %lu, %lu\n", st.st_size, sz);
                goto err_out;
            }
            do_read(fd, read_buf, len, pos, fake_file);
        }
        do_read(fd, read_buf, sz, 0, fake_file);
        close(fd);
    }

    
    return;

err_out:
    printf("hole test err_out\n");
    exit(1);
    return;  
}

void do_underlying_test()
{
    int fd;
    char write_buf[MAX_WRITE_SIZE];
    // char read_buf[FILE_SIZE];
    // char fake_file[FILE_SIZE];

    char* read_buf = malloc(ONE_GB);
    // char* fake_file = malloc(ONE_GB); 

    size_t ret;

    fd = open("/mnt/data/a.txt", O_CREAT | O_RDWR, S_IRWXU);
    
    pwrite(fd, write_buf, 4096, ONE_GB);

    // for(int i = 0; i < 10; i++) {
    //     pwrite(fd, write_buf, 4096, 1048576);
    // }

    ret = pread(fd, read_buf, ONE_GB, 0);
    printf("ret: %lu\n", ret);

    // for(int i = 0; i < 9; i++) {
    //     pread(fd, buf, )
    // }
}

void do_custom_test()
{
    int fd;
    char write_buf[MAX_WRITE_SIZE];
    // char read_buf[FILE_SIZE];
    // char fake_file[FILE_SIZE];

    char* read_buf = malloc(ONE_GB);
    // char* fake_file = malloc(ONE_GB); 

    size_t ret;

    fd = open("/mnt/data/a.txt", O_CREAT | O_RDWR, S_IRWXU);
    
    pwrite(fd, write_buf, 32, 0);

    pwrite(fd, write_buf, 164829, 32);
// [  139.538625] p2cache: xl.meta, {0, 481}, {0, 0}, {0, 0}
// [  139.614758] p2cache: xl.meta, {0, 8192}, {8192, 201}, {0, 0}
// [  139.674174] p2cache: xl.meta, {0, 432}, {0, 0}, {0, 0}
// [  143.642349] p2cache: xl.meta, {0, 1026}, {0, 0}, {0, 0}
// [  146.789197] p2cache: xl.meta, {0, 8192}, {8192, 316}, {0, 0}
//     pwrite(fd, write_buf, 0, 481);
//     pwrite(fd, write_buf, 0, 481);
}

int main() {
    
    // do_append_test();
    // do_overwrite_test();
    // do_overwrite_test1();
    // do_hole_test();
    // do_underlying_test();
    do_custom_test();
    return 0;
}