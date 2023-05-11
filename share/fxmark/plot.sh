#!/bin/bash


LAST_DIR=$(ls -t logs/ | head -n 1)
# echo $LAST_DIR

# bin/plotter.py --ty sc --log logs/2022-10-27-01-00-40.976490/fxmark.log --out out
bin/plotter.py --ty sc --log logs/${LAST_DIR}/fxmark.log --out out