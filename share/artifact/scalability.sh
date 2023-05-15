#!/bin/bash

rm -f share/tmp/scalability.log 

# run experiments
share/fxmark/bin/run-fxmark.py --bench_type scalability --log_dir share/tmp

# parse log, generate csv
share/artifact/log_parser.py --log_path share/tmp/scalability.log --config_path share/artifact/log_parser_scalability.config --output_path share/output/scalability.csv

# plot figure, you can see the figure under share/output/
share/artifact/plot/scalability.py