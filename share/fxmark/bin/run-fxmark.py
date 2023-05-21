#!/usr/bin/env python3
import os
import sys
import signal
import subprocess
import datetime
import tempfile
import pdb
from os.path import join
from perfmon import PerfMon
import subprocess
import argparse

from inspect import getframeinfo, stack

def debuginfo():
    caller = getframeinfo(stack()[2][0])
    print("%s:%d" % (caller.filename, caller.lineno)) # python3 syntax print

def get_size(size_str):
    last_char = size_str[-1]
    if last_char.isalpha() == False:
        return int(size_str)
    
    if last_char.lower() == 'k':
        return int(size_str[:-1]) * 1024
    elif last_char.lower() == 'm':
        return int(size_str[:-1]) * 1024 * 1024
    elif last_char.lower() == 'g':
        return int(size_str[:-1]) * 1024 * 1024 * 1024
    else:
        return 0


CUR_DIR = os.path.abspath(os.path.dirname(__file__))

try:
    import cpupol
except ImportError:
    print("No cpupolicy for this machine.")
    print("Do \'make\' at %s\n"
          % os.path.normpath(os.path.join(CUR_DIR, "..")))
    raise

def catch_ctrl_C(sig, frame):
    print("Umount a testing file system. Please wait.")

class Runner(object):
    # media path
    LOOPDEV = "/dev/loop99"
    NVMEDEV = "/dev/nvme0n1pX"
    HDDDEV  = "/dev/sdX"
    SSDDEV  = "/dev/sdb"
    PMEMDEV = "/dev/pmem0"
    DRAMDEV = "/dev/pmem1"

    # test core granularity
    CORE_FINE_GRAIN   = 0
    CORE_COARSE_GRAIN = 1

    def __init__(self, \
                 core_grain = CORE_COARSE_GRAIN, \
                 pfm_lvl = PerfMon.LEVEL_LOW, \
                 run_filter = ("*", "*", "*", "*", "*"), \
                 log_dir = "/tmp", bench_type = None):
        # run config
        self.CORE_GRAIN    = core_grain
        self.PERFMON_LEVEL = pfm_lvl
        self.FILTER        = run_filter # media, fs, bench, ncore, directio
        self.DRYRUN        = False
        self.DEBUG_OUT     = False

        # bench config
        self.DISK_SIZE     = "128G"
        self.DURATION      = 30 # seconds
        self.DIRECTIOS     = ["bufferedio", "directio"]  # enable directio except tmpfs -> nodirectio 
        self.MEDIA_TYPES   = ["mem", "pmem", "ssd"] # ["ssd", "hdd", "nvme", "mem"]
#        self.FS_TYPES      = [
        self.FS_TYPES      = [
                                "tmpfs",
                                "p2cache",
                                "nova",
                                "ext4_dax", "xfs_dax", "ext4", "xfs", "ext4_dj"
                            #   "ext4", "ext4_no_jnl", "ext4_data"
                            #   "xfs",
                            #   "btrfs", "f2fs",
                              # "jfs", "reiserfs", "ext2", "ext3",
        ]
        # self.BENCH_TYPES   = []
        self.BENCH_BG_SFX   = "_bg"

        # path config
        self.ROOT_NAME      = "root"
        self.LOGD_NAME      = "../logs"
        self.FXMARK_NAME    = "fxmark"
        self.FILEBENCH_NAME = "run-filebench.py"
        self.DBENCH_NAME    = "run-dbench.py"
        self.PERFMN_NAME    = "perfmon.py"

        # fs config
        self.HOWTO_MOUNT = {
            "tmpfs":self.mount_tmpfs,
            "ext2":self.mount_anyfs,
            "ext3":self.mount_anyfs,
            "ext4":self.mount_anyfs,
            "ext4_no_jnl":self.mount_ext4_no_jnl,
            "xfs":self.mount_anyfs,
            "btrfs":self.mount_anyfs,
            "f2fs":self.mount_anyfs,
            "jfs":self.mount_anyfs,
            "reiserfs":self.mount_anyfs,
            "nova": self.mount_nova,
            "p2cache": self.mount_p2cache,
            "ext4_dax": self.mount_ext4_dax,
            "xfs_dax": self.mount_xfs_dax,
            "ext4_dj": self.mount_ext4_dj,
        }
        self.HOWTO_MKFS = {
            "ext2":"-F",
            "ext3":"-F",
            "ext4":"-F",
            "ext4_no_jnl":"-F",
            "xfs":"-f",
            "btrfs":"-f",
            "jfs":"-q",
            "reiserfs":"-q",
        }

        # media config
        self.HOWTO_INIT_MEDIA = {
            "mem":self.init_mem_disk,
            "nvme":self.init_nvme_disk,
            "ssd":self.init_ssd_disk,
            "hdd":self.init_hdd_disk,
            "pmem":self.init_pmem_disk,
            "dram":self.init_dram_disk,
        }

        # misc. setup
        self.redirect    = subprocess.PIPE if not self.DEBUG_OUT else None
        self.dev_null    = open("/dev/null", "a") if not self.DEBUG_OUT else None
        self.npcpu       = cpupol.PHYSICAL_CHIPS * cpupol.CORE_PER_CHIP
        self.nhwthr      = self.npcpu * cpupol.SMT_LEVEL
        self.ncores      = self.get_ncores()
        self.test_root   = os.path.normpath(
            os.path.join(CUR_DIR, self.ROOT_NAME))
        self.fxmark_path = os.path.normpath(
            os.path.join(CUR_DIR, self.FXMARK_NAME))
        self.filebench_path = os.path.normpath(
            os.path.join(CUR_DIR, self.FILEBENCH_NAME))
        self.dbench_path = os.path.normpath(
            os.path.join(CUR_DIR, self.DBENCH_NAME))
        self.tmp_path = os.path.normpath(
            os.path.join(CUR_DIR, ".tmp"))
        self.disk_path = os.path.normpath(
            os.path.join(self.tmp_path, "disk.img"))
        self.perfmon_start = "%s start" % os.path.normpath(
            os.path.join(CUR_DIR, self.PERFMN_NAME))
        self.perfmon_stop = "%s stop" % os.path.normpath(
            os.path.join(CUR_DIR, self.PERFMN_NAME))
        self.nova_module_path = os.path.normpath("../nova/nova.ko")
        self.p2cache_module_path = os.path.normpath("../p2cache/p2cache.ko")
        self.perfmon_log = ""
        self.log_dir     = log_dir
        self.log_path    = ""
        self.umount_hook = []
        self.active_ncore = -1
        self.bench_type = bench_type

    def log_start(self):
        # self.log_dir = os.path.normpath(
        #     os.path.join(CUR_DIR, self.LOGD_NAME,
        #                  str(datetime.datetime.now()).replace(' ','-').replace(':','-')))
        self.log_path = os.path.normpath( os.path.join(self.log_dir, "%s.log" % self.bench_type))
        # self.exec_cmd("mkdir -p " + self.log_dir, self.dev_null)

        self.log_fd = open(self.log_path, "ba", buffering=0)
        p = self.exec_cmd("echo -n \"### SYSTEM         = \"; uname -a", self.redirect)
        if self.redirect:
            for l in p.stdout.readlines():
                self.log(l.decode("utf-8").strip())
        self.log("### DISK_SIZE      = %s"   % self.DISK_SIZE)
        self.log("### DURATION       = %ss"  % self.DURATION)
        self.log("### DIRECTIO       = %s"   % ','.join(self.DIRECTIOS))
        self.log("### MEDIA_TYPES    = %s"   % ','.join(self.MEDIA_TYPES))
        self.log("### FS_TYPES       = %s"   % ','.join(self.FS_TYPES))
        self.log("### BENCH_TYPES    = %s"   % ','.join(self.BENCH_TYPES))
        self.log("### NCORES         = %s"   % 
                 ','.join(map(lambda c: str(c), self.ncores)))
        self.log("### CORE_SEQ       = %s" % 
                 ','.join(map(lambda c: str(c), cpupol.seq_cores)))
        self.log("\n")
        self.log("### MODEL_NAME     = %s" % cpupol.MODEL_NAME)
        self.log("### PHYSICAL_CHIPS = %s" % cpupol.PHYSICAL_CHIPS)
        self.log("### CORE_PER_CHIP  = %s" % cpupol.CORE_PER_CHIP)
        self.log("### SMT_LEVEL      = %s" % cpupol.SMT_LEVEL)
        self.log("\n")

    def log_end(self):
        self.log_fd.close()

    def log(self, log):
        self.log_fd.write((log+'\n').encode('utf-8'))
        print(log)

    def get_ncores(self):
        hw_thr_cnts_map = {
            Runner.CORE_FINE_GRAIN:cpupol.test_hw_thr_cnts_fine_grain,
            Runner.CORE_COARSE_GRAIN:cpupol.test_hw_thr_cnts_coarse_grain,
        }
        ncores = []
        test_hw_thr_cnts = hw_thr_cnts_map.get(self.CORE_GRAIN,
                                               cpupol.test_hw_thr_cnts_fine_grain)
        for n in test_hw_thr_cnts:
            if n > self.npcpu:
                break
            ncores.append(n)
        return ncores

    def exec_cmd(self, cmd, out=None):
        p = subprocess.Popen(cmd, shell=True, stdout=out, stderr=out)
        p.wait()
        return p

    def keep_sudo(self):
        pass
        # self.exec_cmd("sudo -v", self.dev_null)

    def drop_caches(self):
        cmd = ' '.join(["", #sudo
                        os.path.normpath(
                            os.path.join(CUR_DIR, "drop-caches"))])
        self.exec_cmd(cmd, self.dev_null)

    def set_cpus(self, ncore):
        if self.active_ncore == ncore:
            return
        self.active_ncore = ncore
        if ncore == 0:
            ncores = "all"
        else:
            ncores = ','.join(map(lambda c: str(c), cpupol.seq_cores[0:ncore]))
        cmd = ' '.join(["", #sudo
                        os.path.normpath(
                            os.path.join(CUR_DIR, "set-cpus")), 
                        ncores])
        self.exec_cmd(cmd, self.dev_null)

    def add_bg_worker_if_needed(self, bench, ncore):
        if bench.endswith(self.BENCH_BG_SFX):
            ncore = min(ncore + 1, self.nhwthr)
            return (ncore, 1)
        return (ncore, 0)

    def prepre_work(self, ncore):
        self.keep_sudo()
        # self.exec_cmd("sh -c \"echo 0 >/proc/sys/kernel/lock_stat\"", # sudo
        #               self.dev_null)
        
        # self.drop_caches() // 
        # self.exec_cmd("sync", self.dev_null) //
        # self.set_cpus(ncore) // do not uncomment

    def pre_work(self):
        self.exec_cmd("sleep 5")
        self.keep_sudo()
        # self.drop_caches()

    def post_work(self):
        self.keep_sudo()

    def unset_loopdev(self):
        self.exec_cmd(' '.join(["", "losetup", "-d", Runner.LOOPDEV]), # sudo
                      self.dev_null)

    def umount(self, where):
        # debuginfo()
        while True:
            p = self.exec_cmd("umount " + where, self.dev_null) # sudo
            if p.returncode != 0:
                break
        (umount_hook, self.umount_hook) = (self.umount_hook, [])
        map(lambda hook: hook(), umount_hook);

    def init_mem_disk(self):
        self.unset_loopdev()
        self.umount(self.tmp_path)
        self.unset_loopdev()
        self.exec_cmd("mkdir -p " + self.tmp_path, self.dev_null)
        if not self.mount_tmpfs("mem", "tmpfs", self.tmp_path):
            return False;
        self.exec_cmd("dd if=/dev/zero of=" 
                      + self.disk_path +  " bs=1G count=128",
                      self.dev_null)
        p = self.exec_cmd(' '.join(["", "losetup", # sudo
                                    Runner.LOOPDEV, self.disk_path]), 
                          self.dev_null)
        if p.returncode == 0:
            self.umount_hook.append(self.deinit_mem_disk)
        return (p.returncode == 0, Runner.LOOPDEV)

    def deinit_mem_disk(self):
        self.unset_loopdev()
        self.umount(self.tmp_path)

    def init_nvme_disk(self):
        return (os.path.exists(Runner.NVMEDEV), Runner.NVMEDEV)

    def init_ssd_disk(self):
        return (os.path.exists(Runner.SSDDEV), Runner.SSDDEV)

    def init_hdd_disk(self):
        return (os.path.exists(Runner.HDDDEV), Runner.HDDDEV)

    def init_pmem_disk(self):
        return (os.path.exists(Runner.PMEMDEV), Runner.PMEMDEV)
    
    def init_dram_disk(self):
        return (os.path.exists(Runner.DRAMDEV), Runner.DRAMDEV)

    def init_media(self, media):
        _init_media = self.HOWTO_INIT_MEDIA.get(media, None)
        if not _init_media:
            return (False, None)
        (rc, dev_path) = _init_media()
        return (rc, dev_path)

    def mount_tmpfs(self, media, fs, mnt_path):
        p = self.exec_cmd("mount -t tmpfs -o mode=0777,size=" # sudo
                          + self.DISK_SIZE + " none " + mnt_path,
                          self.dev_null)
        return p.returncode == 0

    def mount_anyfs(self, media, fs, mnt_path):
        (rc, dev_path) = self.init_media(media)
        if not rc:
            return False

        p = self.exec_cmd("mkfs." + fs # sudo
                          + " " + self.HOWTO_MKFS.get(fs, "")
                          + " " + dev_path,
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd(' '.join(["mount -t", fs, #sudo
                                    dev_path, mnt_path]),
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd("chmod 777 " + mnt_path, # sudo
                          self.dev_null)
        if p.returncode != 0:
            return False
        return True

    def mount_ext4_no_jnl(self, media, fs, mnt_path):
        (rc, dev_path) = self.init_media(media)
        if not rc:
            return False

        p = self.exec_cmd("mkfs.ext4" # sudo
                          + " " + self.HOWTO_MKFS.get(fs, "")
                          + " " + dev_path,
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd("tune2fs -O ^has_journal %s" % dev_path, # sudo
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd(' '.join(["mount -t ext4", #sudo
                                    dev_path, mnt_path]),
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd("chmod 777 " + mnt_path, # sudo
                          self.dev_null)
        if p.returncode != 0:
            return False
        return True
    
    def mount_nova(self, media, fs, mnt_path):
        (rc, dev_path) = self.init_media(media)
        if not rc:
            return False

        p = self.exec_cmd(' '.join(["mount -t NOVA -o init,dax,data_cow", #sudo
                                    dev_path, mnt_path]),
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd("chmod 777 " + mnt_path, # sudo
                          self.dev_null)
        if p.returncode != 0:
            return False
        return True 

    def mount_p2cache(self, media, fs, mnt_path):
        (rc, dev_path) = self.init_media(media)
        if not rc:
            return False

        p = self.exec_cmd(' '.join(["mount -t p2cache -o init", #sudo
                                    dev_path, mnt_path]),
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd("chmod 777 " + mnt_path, # sudo
                          self.dev_null)
        if p.returncode != 0:
            return False
        return True

    def mount_ext4_dax(self, media, fs, mnt_path):
        # mkfs.ext4 -F /dev/pmem1 && mount -t ext4 -o dax /dev/pmem1 /mnt/data
        (rc, dev_path) = self.init_media(media)
        if not rc:
            return False

        p = self.exec_cmd("mkfs.ext4 -F" # sudo
                          + " " + dev_path,
                          self.dev_null)

        p = self.exec_cmd(' '.join(["mount -t ext4 -o dax", #sudo
                                    dev_path, mnt_path]),
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd("chmod 777 " + mnt_path, # sudo
                          self.dev_null)
        if p.returncode != 0:
            return False
        return True

    def mount_ext4_dj(self, media, fs, mnt_path):
        # mkfs.ext4 -F /dev/pmem1 && mount -t ext4 -o data=journal /dev/pmem1 /mnt/data
        (rc, dev_path) = self.init_media(media)
        if not rc:
            return False

        p = self.exec_cmd("mkfs.ext4 -F" # sudo
                          + " " + dev_path,
                          self.dev_null)

        p = self.exec_cmd(' '.join(["mount -t ext4 -o data=journal", #sudo
                                    dev_path, mnt_path]),
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd("chmod 777 " + mnt_path, # sudo
                          self.dev_null)
        if p.returncode != 0:
            return False
        return True

    def mount_xfs_dax(self, media, fs, mnt_path):
        # mkfs.xfs -m reflink=0 -f /dev/pmem1 && mount -t xfs -o dax /dev/pmem1 /mnt/data
        (rc, dev_path) = self.init_media(media)
        if not rc:
            return False

        p = self.exec_cmd("mkfs.xfs -m reflink=0 -f" # sudo
                          + " " + dev_path,
                          self.dev_null)

        p = self.exec_cmd(' '.join(["mount -t xfs -o dax", #sudo
                                    dev_path, mnt_path]),
                          self.dev_null)
        if p.returncode != 0:
            return False
        p = self.exec_cmd("chmod 777 " + mnt_path, # sudo
                          self.dev_null)
        if p.returncode != 0:
            return False
        return True

    def mount(self, media, fs, mnt_path):
        mount_fn = self.HOWTO_MOUNT.get(fs, None)
        if not mount_fn:
            return False;

        self.umount(mnt_path)
        self.exec_cmd("sleep 5")
        self.exec_cmd("mkdir -p " + mnt_path, self.dev_null)
        return mount_fn(media, fs, mnt_path)

    def _match_config(self, key1, key2):
        for (k1, k2) in zip(key1, key2):
            if k1 == "*" or k2 == "*":
                continue
            if k1 != k2:
                return False
        return True

    def gen_config(self):
        for ncore in sorted(self.ncores, reverse=True):
            for bench in self.BENCH_TYPES:
                for media in self.MEDIA_TYPES:
                    for dio in self.DIRECTIOS:
                        for fs in self.FS_TYPES:
                            if fs == "tmpfs" and media != "mem":
                                continue
                            if (fs == "nova" or fs == "p2cache") and (media != "pmem" and media != "dram"):
                                continue
                            mount_fn = self.HOWTO_MOUNT.get(fs, None)
                            if not mount_fn:
                                continue
                            if self._match_config(self.FILTER, \
                                                  (media, fs, bench, str(ncore), dio)):
                                yield(media, fs, bench, ncore, dio)

    def fxmark_env(self):
        env = ' '.join(["PERFMON_LEVEL=%s" % self.PERFMON_LEVEL,
                        "PERFMON_LDIR=%s"  % self.log_dir,
                        "PERFMON_LFILE=%s" % self.perfmon_log])
        return env

    def get_bin_type(self, bench):
        if bench.startswith("filebench_"):
            return (self.filebench_path, bench[len("filebench_"):])
        if bench.startswith("dbench_"):
            return (self.dbench_path, bench[len("dbench_"):])
        return (self.fxmark_path, bench)

    def fxmark(self, media, fs, bench, ncore, nfg, nbg, dio):
        (bin, type) = self.get_bin_type(bench)
        pos = type.find("NAME_")
        if pos != -1:
            bench_name = type[pos+5:]
            type = type[:pos-1]

        self.perfmon_log = os.path.normpath(
            os.path.join(self.log_dir,
                         '.'.join([media, fs, type, str(nfg), "pm"])))

        print(self.perfmon_log)
        
        directio = '1' if dio == "directio" else '0'

        if directio == '1':
            if fs == "tmpfs": 
                print("# INFO: DirectIO under tmpfs disabled by default")
                directio='0';
            else: 
                print("# INFO: DirectIO Enabled")

        cmd = ' '.join([self.fxmark_env(),
                        bin,
                        "--ncore", str(ncore),
                        "--nbg",  str(nbg),
                        "--duration", str(self.DURATION) if bench_name != "Fileserver" else "10",
                        # "--directio", directio,
                        "--root", self.test_root,
                        "--profbegin", "\"%s\"" % self.perfmon_start,
                        "--profend",   "\"%s\"" % self.perfmon_stop,
                        "--proflog", self.perfmon_log])
        
        if type.startswith("DATA"):
            d_as, d_ss, d_ts = 4096, 4096, 8589934592
            d_p_as, d_p_ss, d_p_ts = 4096, 4096, 8589934592
            op, sync = 0, 0

            data_args = type.split(':')

            for arg in data_args:
                if arg.startswith('AS'):
                    d_as = get_size(arg[3:])
                elif arg.startswith('SS'):
                    d_ss = get_size(arg[3:])
                elif arg.startswith('TS'):
                    d_ts = get_size(arg[3:])
                elif arg.startswith('PAS'):
                    d_p_as = get_size(arg[4:])
                elif arg.startswith('PSS'):
                    d_p_ss = get_size(arg[4:])
                elif arg.startswith('PTS'):
                    d_p_ts = get_size(arg[4:])
                elif arg == 'SYNC':
                    sync = 1
                elif arg == 'W':
                    op = 1

            cmd = ' '.join([cmd,
                            "--type", "DATA",
                            "--d_as", str(d_as),
                            "--d_ss", str(d_ss),
                            "--d_ts", str(d_ts),
                            "--d_p_as", str(d_p_as),
                            "--d_p_ss", str(d_p_ss),
                            "--d_p_ts", str(d_p_ts),
                            "--sync", str(sync),
                            "--op", str(op)])
        elif type.startswith("META"):
            m_amount = 1048576
            op, sync = 2, 0

            meta_args = type.split(':')

            for arg in meta_args:
                if arg == 'SYNC':
                    sync = 1
                elif arg == 'CREATE':
                    op = 2
                elif arg == 'MKDIR':
                    op = 3
                elif arg == 'UNLINK':
                    op = 4
                elif arg == 'RMDIR':
                    op = 5
                elif arg == 'LINK':
                    op = 6
                elif arg == 'RENAME':
                    op = 7
                elif arg.startswith('AM'):
                    m_amount = get_size(arg[3:])

            cmd = ' '.join([cmd,
                            "--type", "META",
                            "--m_amount", str(m_amount),
                            "--sync", str(sync),
                            "--op", str(op)])
        else:
            cmd = ' '.join([cmd, "--type", type])

        self.log("[bench_name]:[%s]"%bench_name)
        # print(cmd)
        p = self.exec_cmd(cmd, self.redirect)
        if self.redirect:
            for l in p.stdout.readlines():
                self.log(l.decode("utf-8").strip())

    def fxmark_cleanup(self):
        cmd = ' '.join([self.fxmark_env(),
                        "%s; rm -f %s/*.pm" % (self.perfmon_stop, self.log_dir)])
        self.exec_cmd(cmd)
        # self.exec_cmd("sh -c \"echo 0 >/proc/sys/kernel/lock_stat\"", # sudo
        #               self.dev_null)

    def run(self):

        self.exec_cmd("dmesg -n 1")
        try:
            cnt = -1
            self.log_start()
            if ("nova" in self.FS_TYPES) and module_loaded("nova") != True:
                p = self.exec_cmd("insmod " + self.nova_module_path)
                if p.returncode != 0:
                    self.log("# Fail to load nova kernel module")
                    return

            if ("p2cache" in self.FS_TYPES) and module_loaded("p2cache") != True:
                p = self.exec_cmd("insmod " + self.p2cache_module_path)
                if p.returncode != 0:
                    self.log("# Fail to load p2cache kernel module")
                    return


            for (cnt, (media, fs, bench, ncore, dio)) in enumerate(self.gen_config()):
                (ncore, nbg) = self.add_bg_worker_if_needed(bench, ncore)
                nfg = ncore - nbg

                if self.DRYRUN:
                    self.log("## %s:%s:%s:%s:%s" % (media, fs, bench, nfg, dio))
                    continue

                self.prepre_work(ncore)
                if not self.mount(media, fs, self.test_root):
                    self.log("# Fail to mount %s on %s." % (fs, media))
                    continue

                # self.exec_cmd("mount | grep /dev/pmem0")
                self.log("-----------start-----------")
                self.log("## %s:%s:%s:%s:%s" % (media, fs, bench, nfg, dio))
                self.pre_work()
                # self.exec_cmd("du -sh %s" % (self.test_root))
                # self.exec_cmd("tree %s" % (self.test_root))
                self.fxmark(media, fs, bench, ncore, nfg, nbg, dio)
                self.log("[media]:[%s],[fs]:[%s]" % (media, fs))
                self.log("---------------------------")
                # self.exec_cmd("du -sh %s" % (self.test_root))
                # self.exec_cmd("tree %s" % (self.test_root))
                self.post_work()
            self.log("### NUM_TEST_CONF  = %d" % (cnt + 1))
        finally:
            signal.signal(signal.SIGINT, catch_ctrl_C)
            self.log_end()
            self.fxmark_cleanup()
            self.umount(self.test_root)
            # self.set_cpus(0)

def confirm_media_path():
    print("%" * 80)
    print("%% WARNING! WARNING! WARNING! WARNING! WARNING!")
    print("%" * 80)
    yn = input("All data in %s, %s, %s and %s will be deleted. Is it ok? [Y,N]: "
            % (Runner.HDDDEV, Runner.SSDDEV, Runner.NVMEDEV, Runner.LOOPDEV))
    if yn != "Y":
        print("Please, check Runner.LOOPDEV and Runner.NVMEDEV")
        exit(1)
    yn = input("Are you sure? [Y,N]: ")
    if yn != "Y":
        print("Please, check Runner.LOOPDEV and Runner.NVMEDEV")
        exit(1)
    print("%" * 80)
    print("\n\n")


def module_loaded(module_name):
    """Checks if module is loaded"""
    lsmod_proc = subprocess.Popen(['lsmod'], stdout=subprocess.PIPE)
    grep_proc = subprocess.Popen(['grep', module_name], stdin=lsmod_proc.stdout)
    grep_proc.communicate()  # Block until finished
    return grep_proc.returncode == 0

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='benchmark')
    parser.add_argument('-l', '--log_dir', type=str, default= "test.log",
                        help='path to log')
    parser.add_argument('-b', '--bench_type', type=str, default= "",
                        help='bench type, e.g., meta_without_fsync')

    args = parser.parse_args()

    base_media_and_fs = [
        # (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("mem", "tmpfs", "*", "1", "bufferedio")),
        (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("ssd", "ext4", "*", "1", "bufferedio")),
        (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("ssd", "ext4_dj", "*", "1", "bufferedio")),
        (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("ssd", "xfs", "*", "1", "bufferedio")),
        (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "ext4_dax", "*", "1", "bufferedio")),
        (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "xfs_dax", "*", "1", "bufferedio")),
        (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "nova", "*", "1", "bufferedio")),
        (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "p2cache", "*", "1", "bufferedio")),
    ]


    amount = 1000000
    ts = '8G'

    bench_types = {
        "meta_without_fsync": [
            "META:CREATE:AM_%d:NAME_CREATE" % amount,
            "META:MKDIR:AM_%d:NAME_MKDIR" % amount,
            "META:UNLINK:AM_%d:NAME_UNLINK" % amount,
            "META:RMDIR:AM_%d:NAME_RMDIR" % amount,
            "META:LINK:AM_%d:NAME_LINK" % amount,
            "META:RENAME:AM_%d::NAME_RENAME" % amount,
        ],
        "meta_with_fsync": [
            "META:CREATE:SYNC:AM_%d:NAME_CREATE" % amount,
            "META:MKDIR:SYNC:AM_%d:NAME_MKDIR" % amount,
            "META:UNLINK:SYNC:AM_%d:NAME_UNLINK" % amount,
            "META:RMDIR:SYNC:AM_%d:NAME_RMDIR" % amount,
            "META:LINK:SYNC:AM_%d:NAME_LINK" % amount,
            "META:RENAME:SYNC:AM_%d:NAME_RENAME" % amount,
        ],
        "data_without_fdatasync": [    
            # append
            "DATA:W:AS_100:SS_100:TS_%s:PTS_0:NAME_A_100B" % ts, # append 100B (access_size 100B, step_size 100B, total_size 8G)
            "DATA:W:AS_1K:SS_1K:TS_%s:PTS_0:NAME_A_1KB" % ts, # append 1K
            "DATA:W:AS_2K:SS_2K:TS_%s:PTS_0:NAME_A_2KB" % ts, # append 2K
            "DATA:W:AS_4K:SS_4K:TS_%s:PTS_0:NAME_A_4KB" % ts, # append 4K
            "DATA:W:AS_16K:SS_16K:TS_%s:PTS_0:NAME_A_16KB" % ts, # append 16K
            "DATA:W:AS_64K:SS_64K:TS_%s:PTS_0:NAME_A_64KB" % ts, # append 64K
            "DATA:W:AS_1M:SS_1M:TS_%s:PTS_0:NAME_A_1MB" % ts, # append 1M
            # overwrite
            "DATA:W:AS_100:SS_100:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_O_100B" % (ts, ts),
            "DATA:W:AS_1K:SS_1K:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_O_1KB" % (ts, ts),
            "DATA:W:AS_2K:SS_2K:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_O_2KB" % (ts, ts),
        ],
        "data_with_fdatasync": [
            # append 
            "DATA:W:AS_100:SS_100:TS_%s:PTS_0:SYNC:NAME_A_100B" % ts,
            "DATA:W:AS_1K:SS_1K:TS_%s:PTS_0:SYNC:NAME_A_1KB" % ts,
            "DATA:W:AS_2K:SS_2K:TS_%s:PTS_0:SYNC:NAME_A_2KB" % ts,
            "DATA:W:AS_4K:SS_4K:TS_%s:PTS_0:SYNC:NAME_A_4KB" % ts,
            "DATA:W:AS_16K:SS_16K:TS_%s:PTS_0:SYNC:NAME_A_16KB" % ts,
            "DATA:W:AS_64K:SS_64K:TS_%s:PTS_0:SYNC:NAME_A_64KB" % ts,
            "DATA:W:AS_1M:SS_1M:TS_%s:PTS_0:SYNC:NAME_A_1MB" % ts,
            # overwrite
            "DATA:W:AS_100:SS_100:TS_%s:PAS_32K:PSS_32K:PTS_%s:SYNC:NAME_O_100B" % (ts, ts),
            "DATA:W:AS_1K:SS_1K:TS_%s:PAS_32K:PSS_32K:PTS_%s:SYNC:NAME_O_1KB" % (ts, ts),
            "DATA:W:AS_2K:SS_2K:TS_%s:PAS_32K:PSS_32K:PTS_%s:SYNC:NAME_O_2KB" % (ts, ts),
        ],
        "read": [
            "DATA:R:AS_100:SS_100:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_100B" % (ts, ts),
            "DATA:R:AS_1K:SS_1K:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_1KB" % (ts, ts),
            "DATA:R:AS_2K:SS_2K:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_2KB" % (ts, ts),
            "DATA:R:AS_4K:SS_4K:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_4KB" % (ts, ts),
            "DATA:R:AS_16K:SS_16K:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_16KB" % (ts, ts),
            "DATA:R:AS_64K:SS_64K:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_64KB" % (ts, ts),
            "DATA:R:AS_1M:SS_1M:TS_%s:PAS_32K:PSS_32K:PTS_%s:NAME_1MB" % (ts, ts),
        ],
        "read_hole": [
            "DATA:R:AS_4K:SS_4K:TS_%s:PAS_3k:PSS_4K:PTS_%s:NAME_HOLE_1KB" % (ts, ts),
            "DATA:R:AS_4K:SS_4K:TS_%s:PAS_2k:PSS_4K:PTS_%s:NAME_HOLE_2KB" % (ts, ts),
        ],
        "scalability": [
            "DATA:W:AS_4K:SS_4K:TS_%s:PTS_0:SYNC:NAME_4KB" % ts,
        ],
        "filebench": [
            "filebench_webproxy:NAME_Webproxy",
            "filebench_varmail:NAME_Varmail",
            "filebench_fileserver:NAME_Fileserver",
        ]
    }
    
    media_and_fs_config = {
        "meta_without_fsync": base_media_and_fs + [(Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("mem", "tmpfs", "*", "1", "bufferedio"))],
        "meta_with_fsync": base_media_and_fs + [(Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("mem", "tmpfs", "*", "1", "bufferedio"))],
        "data_without_fdatasync": base_media_and_fs,
        "data_with_fdatasync": base_media_and_fs,
        "read": base_media_and_fs,
        "read_hole": [(Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "p2cache", "*", "1", "bufferedio"))],
        "scalability": [
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("mem", "tmpfs", "*", "*", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("ssd", "ext4", "*", "*", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("ssd", "ext4_dj", "*", "*", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("ssd", "xfs", "*", "*", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "ext4_dax", "*", "*", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "xfs_dax", "*", "*", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "nova", "*", "*", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "p2cache", "*", "*", "bufferedio"))
        ],
        "filebench": [
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("mem", "tmpfs", "*", "8", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("ssd", "ext4", "*", "8", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("ssd", "ext4_dj", "*", "8", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("ssd", "xfs", "*", "8", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "ext4_dax", "*", "8", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "xfs_dax", "*", "8", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "nova", "*", "8", "bufferedio")),
            (Runner.CORE_FINE_GRAIN, PerfMon.LEVEL_LOW, ("pmem", "p2cache", "*", "8", "bufferedio"))
        ]
    }

    Runner.BENCH_TYPES = bench_types[args.bench_type]

    for c in media_and_fs_config[args.bench_type]:
        runner = Runner(c[0], c[1], c[2], log_dir = args.log_dir, bench_type = args.bench_type)
        runner.run()