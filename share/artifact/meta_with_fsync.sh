#!/bin/bash

ulimit -n 1048576

rm -f share/tmp/meta_with_fsync.log 

# run experiments
share/fxmark/bin/run-fxmark.py --bench_type meta_with_fsync --log_dir share/tmp

# parse log, generate csv
share/artifact/log_parser.py --log_path share/tmp/meta_with_fsync.log --config_path share/artifact/log_parser.config --output_path share/output/meta_with_fsync.csv

# plot figure, you can see the figure under share/output/
share/artifact/plot/meta_with_fsync.py