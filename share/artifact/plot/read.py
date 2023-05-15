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
"#e6f5c9", # TMPFS
"#fff2ae", # XFS
"#f1e2cc", # XFS-DAX
    
    
]

def calc_pos(total_nr, idx):
    width = 0.7 / total_nr
    offset = (idx - (total_nr - 1) / 2) * width
    return offset, width

fig, ax = plt.subplots(figsize = (12,4))

df = pd.read_csv("share/output/read.csv", delimiter = ",", encoding='utf8')



data_names = ["ext4_ops", "ext4_dax_ops", "ext4_dj_ops", "nova_ops", 
         "p2cache_ops", "xfs_ops", "xfs_dax_ops"]

label_names = ["Ext4", "Ext4-DAX", "Ext4-DJ", "NOVA",
        "p2Cache", "XFS", "XFS-DAX"]

xaxis = ["100B", "1KB", "2KB", "4KB", "16KB", "64KB"]

x = np.arange(len(xaxis), dtype = float)

df = df.loc[df['bench_name'].isin(xaxis)].copy()

df['bench_name'] = df['bench_name'].astype("category")
df['bench_name'] = df['bench_name'].cat.set_categories(xaxis)
df = df.sort_values('bench_name')


bar_num = len(data_names)

x[4] += 2.0 * 0.7 / bar_num
x[5] += 2.0 * 0.7 / bar_num

for idx, i in enumerate(data_names):
    offset, wid = calc_pos(bar_num, idx)
    ax.bar(x + offset, df[i] / 1000000, label = label_names[idx], color = colors[idx], width = wid, edgecolor='black', hatch = patterns[idx])

offset, wid = calc_pos(bar_num, 6)
# ax.bar(x[3] + offset + wid, df["p2Cache (1K holes on PM)"][3] / 1000000, label="p2Cache (1K holes on PM)", color = "#D75455", width = wid, edgecolor='black', hatch = "*")
# ax.bar(x[3] + offset + 2 * wid, df["p2Cache (2K holes on PM)"][3] / 1000000, label="p2Cache (2K holes on PM)", color = "#FCFAF2", width = wid, edgecolor='black', hatch = "..")
    
ax.set_yscale('log')

ax.grid(axis= 'y',linestyle='--')
ax.tick_params(axis='both', labelsize=16)
ax.legend(ncol = 3, fontsize = 16, frameon=False, loc = "upper right")
ax.set_ylabel("Operations per second\n(Mops/Sec)", fontsize = 16)
# ax.set_xlabel("Thread #", fontsize = 16)
x[3] += 0.7 / bar_num
ax.set_xticks(x, xaxis)
ax.set_yticks([0.05, 0.1 , 0.2, 0.3, 0.5, 1, 3, 7, 30], ["0.05", "0.1", "0.2", "0.3", "0.5", "1", "3", "7", ""])
fig.tight_layout(pad =0)
foo_fig = plt.gcf() # 'get current figure'
foo_fig.savefig('share/output/read.png',bbox_inches='tight', format='png', dpi=1000,pad_inches=0.0)