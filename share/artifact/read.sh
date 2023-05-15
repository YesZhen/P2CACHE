#!/bin/bash

rm -f share/tmp/read.log 

# run experiments
share/fxmark/bin/run-fxmark.py --bench_type read --log_dir share/tmp

# parse log, generate csv
share/artifact/log_parser.py --log_path share/tmp/read.log --config_path share/artifact/log_parser.config --output_path share/output/read.csv

# plot figure, you can see the figure under share/output/
share/artifact/plot/read.py