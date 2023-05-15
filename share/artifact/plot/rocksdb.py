#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import os
import pandas as pd

patterns = [ "//" , "\\\\" , "||||" , "++" , "//-", "\\\\-" , "||-", "++\\", "O", "." ]

colors = [
# "#b2182b",
# "#ef8a62",
# "#fddbc7",
# "#ffffff",
# "#A5DEE4",
# "#e0e0e0",
# "#999999",
# "#4d4d4d",

"#b3e2cd", # ext4
"#fdcdac", # ext4_dax
"#cbd5e8", # ext4_dj
"#f4cae4", # nova
"black",   # p2Cache
# "#e6f5c9", # TMPFS
"#fff2ae", # XFS
"#f1e2cc", # XFS-DAX
    
    
]

def calc_pos(total_nr, idx):
    width = 0.7 / total_nr
    offset = (idx - (total_nr - 1) / 2) * width
    return offset, width

fig, ax = plt.subplots(figsize = (7,4))
df = pd.read_csv("share/output/rocksdb.csv", delimiter = ",", encoding='utf8')

data_names = ["ext4_ops", "ext4_dax_ops", "ext4_dj_ops", "nova_ops",
        "p2cache_ops", "xfs_ops", "xfs_dax_ops"]

label_names = ["Ext4", "Ext4-DAX", "Ext4-DJ", "NOVA",
        "p2Cache", "XFS", "XFS-DAX"]

xaxis = ["insert_100B", "insert_1KB", "read_100B", "read_1KB"]

x = np.arange(len(df["bench_name"]), dtype = float)

df = df.loc[df['bench_name'].isin(xaxis)].copy()

df['bench_name'] = df['bench_name'].astype("category")
df['bench_name'] = df['bench_name'].cat.set_categories(xaxis)
df = df.sort_values('bench_name')

bar_num = len(data_names)

x = np.arange(len(df["bench_name"]), dtype = float)
for idx, i in enumerate(data_names):
    offset, wid = calc_pos(bar_num, idx)
    ax.bar(x + offset, df[i] / 1000000, color = colors[idx], width = wid, edgecolor='black', hatch = patterns[idx])
ax.set_yscale('log')
ax.grid(axis= 'y',linestyle='--')
ax.tick_params(axis='both', labelsize=16)
ax.set_xticks(x, xaxis)
ax.set_yticks([0.001, 0.01, 0.1, 0.2, 0.4, 1, 2], ["0.001", "0.01", "0.1", "0.2", "0.4", "1", "2"])
ax.set_title("(b) RocksDB", fontsize = 16, y= -0.2, x = 0.5)


fig.legend(ncol = 2, fontsize = 16, frameon=False, bbox_to_anchor=(0.85, 1.1))

fig.tight_layout(pad =0)
foo_fig = plt.gcf() # 'get current figure'
foo_fig.savefig('share/output/rocksdb.png',bbox_inches='tight', format='png', dpi=1000,pad_inches=0.0)