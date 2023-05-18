#
# Makefile for the linux NOVA filesystem routines.
#

obj-m += nova.o

nova-y := balloc.o bbuild.o checksum.o dax.o dir.o file.o gc.o inode.o ioctl.o \
	journal.o log.o mprotect.o namei.o parity.o rebuild.o snapshot.o stats.o \
	super.o symlink.o sysfs.o perf.o

build_vm:
	make -C ../linux-5.4.225 M=$(PWD) modules

clean_vm:
	make -C ../linux-5.4.225 M=$(PWD) clean

build_host:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean_host:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
