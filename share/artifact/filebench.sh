#!/bin/bash

ulimit -n 1048576

rm -f share/tmp/filebench.log 

# run experiments
numactl -C 0-7 share/fxmark/bin/run-fxmark.py --bench_type filebench --log_dir share/tmp

# # parse log, generate csv
share/artifact/log_parser.py --log_path share/tmp/filebench.log --config_path share/artifact/log_parser.config --output_path share/output/filebench.csv

# # plot figure, you can see the figure under share/output/
share/artifact/plot/filebench.py