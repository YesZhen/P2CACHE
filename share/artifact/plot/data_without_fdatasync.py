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

"#b3e2cd",
"#fdcdac",
"#cbd5e8",
"#f4cae4",
"black",
"#fff2ae",
"#f1e2cc",
    
    
]

def calc_pos(total_nr, idx):
    width = 0.7 / total_nr
    offset = (idx - (total_nr - 1) / 2) * width
    return offset, width

# axis for append and overwrite
fig, (ax_A, ax_O) = plt.subplots(1, 2, figsize = (16,4), gridspec_kw={'width_ratios': [2, 1]})

df = pd.read_csv("share/output/data_without_fdatasync.csv", delimiter = ",", encoding='utf8')


data_names = ["ext4_ops", "ext4_dax_ops", "ext4_dj_ops", "nova_ops", 
         "p2cache_ops", "xfs_ops", "xfs_dax_ops"]

label_names = ["Ext4", "Ext4-DAX", "Ext4-DJ", "NOVA",
        "p2Cache", "XFS", "XFS-DAX"]

xaxis_A = ["A_100B", "A_1KB", "A_2KB", "A_4KB", "A_16KB", "A_64KB"]
xaxis_O = ["O_100B", "O_1KB", "O_2KB"]

x_A = np.arange(len(xaxis_A))
x_O = np.arange(len(xaxis_O))


df_A = df.loc[df['bench_name'].isin(xaxis_A)].copy()
df_O = df.loc[df['bench_name'].isin(xaxis_O)].copy()

df_A['bench_name'] = df_A['bench_name'].astype("category")
df_O['bench_name'] = df_O['bench_name'].astype("category")
df_A['bench_name'] = df_A['bench_name'].cat.set_categories(xaxis_A)
df_O['bench_name'] = df_O['bench_name'].cat.set_categories(xaxis_O)
df_A = df_A.sort_values('bench_name')
df_O = df_O.sort_values('bench_name')


bar_num = len(data_names)
for idx, i in enumerate(data_names):
    offset, wid = calc_pos(bar_num, idx)
    ax_A.bar(x_A + offset, df_A[i] / 1000000, label = label_names[idx], color = colors[idx], width = wid, edgecolor='black', hatch = patterns[idx])

ax_A.set_xticks(x_A, xaxis_A)


for idx, i in enumerate(data_names):
    offset, wid = calc_pos(bar_num, idx)
    ax_O.bar(x_O + offset, df_O[i] / 1000000, color = colors[idx], width = wid, edgecolor='black', hatch = patterns[idx])

ax_O.set_xticks(x_O, xaxis_O)
    
ax_O.sharey(ax_A)
ax_O.label_outer()
    
ax_A.set_yscale('log')
ax_A.grid(axis= 'y',linestyle='--')
ax_O.grid(axis= 'y',linestyle='--')
ax_A.tick_params(axis='both', labelsize=16)
ax_O.tick_params(axis='both', labelsize=16)
fig.legend(ncol = 7, fontsize = 16, frameon=False, bbox_to_anchor=(0.95, 1.1))
ax_A.set_ylabel("Operations per second\n(Mops/Sec)", fontsize = 16)
ax_A.set_title("(1) Append", fontsize = 16, y= -0.2, x = 0.5)
ax_O.set_title("(2) Overwrite", fontsize = 16, y= -0.2, x = 0.5)
ax_A.set_yticks([0.001, 0.005, 0.01, 0.05, 0.1 , 0.2, 0.5, 1, 2, 4], ["0.001", "0.005", "0.01", "0.05", "0.1", "0.2", "0.5", "1", "2", ""])
fig.tight_layout(pad =0)
foo_fig = plt.gcf() # 'get current figure'
foo_fig.savefig('share/output/data_without_fdatasync.png',bbox_inches='tight', format='png', dpi=1000,pad_inches=0.0)