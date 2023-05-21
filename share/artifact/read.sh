#!/bin/bash

rm -f share/tmp/read.log
rm -f share/tmp/read_hole.log 

# run experiments
share/fxmark/bin/run-fxmark.py --bench_type read --log_dir share/tmp

share/fxmark/bin/run-fxmark.py --bench_type read_hole --log_dir share/tmp

cat share/tmp/read_hole.log >> share/tmp/read.log

# parse log, generate csv
share/artifact/log_parser.py --log_path share/tmp/read.log --config_path share/artifact/log_parser.config --output_path share/output/read.csv

# plot figure, you can see the figure under share/output/
share/artifact/plot/read.py