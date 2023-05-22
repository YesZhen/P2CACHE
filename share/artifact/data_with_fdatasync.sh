#!/bin/bash

ulimit -n 1048576

rm -f share/tmp/data_with_fdatasync.log 

# run experiments
share/fxmark/bin/run-fxmark.py --bench_type data_with_fdatasync --log_dir share/tmp

# parse log, generate csv
share/artifact/log_parser.py --log_path share/tmp/data_with_fdatasync.log --config_path share/artifact/log_parser.config --output_path share/output/data_with_fdatasync.csv

# plot figure, you can see the figure under share/output/
share/artifact/plot/data_with_fdatasync.py