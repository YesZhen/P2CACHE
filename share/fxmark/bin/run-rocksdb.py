#!/usr/bin/env python3
import os
import sys
import signal
import subprocess
import datetime
import tempfile
import optparse
import time
import pdb
from os.path import join

CUR_DIR = os.path.abspath(os.path.dirname(__file__))

class RocksDB(object):
    PRE_SCRIPT = os.path.normpath(os.path.join(CUR_DIR, "turnoff-aslr"))
    PERF_STR = "IO Summary: "

    def __init__(self, type_, ncore_, duration_, root_,
                 profbegin_, profend_, proflog_):
        self.config = None
        self.bench_out = None
        # take configuration parameters
        self.workload = type_
        self.ncore = int(ncore_)
        self.duration = int(duration_)
        self.root = root_
        
        self.perf_msg = None

    def __del__(self):
        # clean up
        try:
            if self.config:
                os.unlink(self.config.name)
            if self.bench_out:
                os.unlink(self.bench_out.name)
        except:
            pass

    def run(self):
        # set up benchmark configuration
        # if not self.generate_config():
        #     return -1
        # run pre-script then sync
        # self._exec_cmd("sudo %s; sync" % FileBench.PRE_SCRIPT).wait()
        self._exec_cmd("%s; sync" % RocksDB.PRE_SCRIPT).wait()
        # start performance profiling
        # self._exec_cmd("%s %s" % (self.profenv, self.profbegin)).wait()
        # run filebench
        self._run_rocksdb()
        # stop performance profiling
        # self._exec_cmd("%s %s" % (self.profenv, self.profend)).wait()
        return 0

    def _run_rocksdb(self):



        if self.workload == 'insert_100B':
            cmd = './share/rocksdb/db_bench --benchmarks="fillrandom" --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --sync --key_size=20 --value_size=100 --memtablerep=skip_list --num=1000000 --threads=1'
        elif self.workload == 'insert_1KB':
            cmd = './share/rocksdb/db_bench --benchmarks="fillrandom" --wal_dir=/mnt/data --db=/mnt/data --disable_auto_compactions --disable_wal=false --sync --key_size=20 --value_size=1024 --memtablerep=skip_list --num=1000000 --threads=1'
        elif self.workload == 'read_100B':
            cmd = 
        cmd = "./db_bench-5.4 -f %s" % self.config.name
        with tempfile.NamedTemporaryFile(delete=False) as self.bench_out:
            # cmd = "sudo filebench -f %s" % self.config.name
            cmd = "./db_bench-5.4 -f %s" % self.config.name
            p = self._exec_cmd(cmd, subprocess.PIPE)
            while True:
                for l in p.stdout.readlines():
                    print(l)
                    self.bench_out.write("#@ ".encode("utf-8"))
                    self.bench_out.write(l)
                    l_str = str(l)
                    idx = l_str.find(FileBench.PERF_STR)
                    if idx != -1:
                        self.perf_msg = l_str[idx+len(FileBench.PERF_STR):]
                # if not p.poll():
                #    break
                if self.perf_msg:
                    break
            self.bench_out.flush()

    def report(self):
        # 65231: 31.114: IO Summary: 34453 ops, 1148.248 ops/s, (177/177 r/w),   4.0mb/s, 420us cpu/op,   5.4ms latency
        work = 0
        work_sec = 0
        for item in self.perf_msg.split(','):
            vk = item.strip().split()
            if len(vk) == 2:
                if vk[1] == "ops":
                    work = vk[0]
                elif vk[1] == "ops/s":
                    work_sec = vk[0]
        

    def _exec_cmd(self, cmd, out=None):
        p = subprocess.Popen(cmd, shell=True, stdout=out, stderr=out)
        return p

if __name__ == "__main__":
    parser = optparse.OptionParser()
    parser.add_option("--type", help="workload name")
    parser.add_option("--ncore", help="number of core")
    parser.add_option("--nbg", help="not used")
    parser.add_option("--duration", help="benchmark time in seconds")
    parser.add_option("--root", help="benchmark root directory")
    # parser.add_option("--profbegin", help="profile begin command")
    # parser.add_option("--profend", help="profile end command")
    # parser.add_option("--proflog", help="profile log path")
    (opts, args) = parser.parse_args()

    # check options
    for opt in vars(opts):
        val = getattr(opts, opt)
        if val == None:
            print("Missing options: %s" % opt)
            parser.print_help()
            exit(1)

    # run benchmark
    filebench = FileBench(opts.type, opts.ncore, opts.duration, opts.root,
                          opts.profbegin, opts.profend, opts.proflog)
    rc = filebench.run()
    filebench.report()
    exit(rc)

