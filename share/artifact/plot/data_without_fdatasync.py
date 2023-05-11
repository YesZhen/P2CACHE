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

fig, (ax, ax1) = plt.subplots(1, 2, figsize = (16,4), gridspec_kw={'width_ratios': [2, 1]})

df = pd.read_csv("share/output/data_without_fdatasync.csv", delimiter = "\t")
# df1 = pd.read_csv("data/fig7a2.txt", delimiter = "\t")
x = np.arange(len(df["bench_name"]))



data_names = ["ext4_ops", "ext4_dax_ops", "ext4_dj_ops", "nova_ops", 
         "p2cache_ops", "xfs_ops", "xfs_dax_ops"]

label_names = ["Ext4", "Ext4-DAX", "Ext4-DJ", "NOVA",
        "p2Cache", "XFS", "XFS-DAX"]
xaxis = ["100B", "1KB", "2KB", "4KB", "16KB", "64KB"]

bar_num = len(data_names)
for idx, i in enumerate(data_names):
    offset, wid = calc_pos(bar_num, idx)
    ax.bar(x + offset, df[i] / 1000000, label = label_names[idx], color = colors[idx], width = wid, edgecolor='black', hatch = patterns[idx])

ax.set_xticks(x, xaxis)
    
# x = np.arange(len(df1["bench_name"]))

for idx, i in enumerate(data_names):
    offset, wid = calc_pos(bar_num, idx)
    ax1.bar(x + offset, df1[i] / 1000000, color = colors[idx], width = wid, edgecolor='black', hatch = patterns[idx])

xaxis = ["100B", "1KB", "2KB"]
ax1.set_xticks(x, xaxis)
    
ax1.sharey(ax)
ax1.label_outer()
    
ax.set_yscale('log')
ax.grid(axis= 'y',linestyle='--')
ax1.grid(axis= 'y',linestyle='--')
ax.tick_params(axis='both', labelsize=16)
ax1.tick_params(axis='both', labelsize=16)
fig.legend(ncol = 7, fontsize = 16, frameon=False, bbox_to_anchor=(0.95, 1.1))
ax.set_ylabel("Operations per second\n(Mops/Sec)", fontsize = 16)
ax.set_title("(1) Append", fontsize = 16, y= -0.2, x = 0.5)
ax1.set_title("(2) Overwrite", fontsize = 16, y= -0.2, x = 0.5)
ax.set_yticks([0.001, 0.005, 0.01, 0.05, 0.1 , 0.2, 0.5, 1, 2, 4], ["0.001", "0.005", "0.01", "0.05", "0.1", "0.2", "0.5", "1", "2", ""])
fig.tight_layout(pad =0)
foo_fig = plt.gcf() # 'get current figure'
foo_fig.savefig('share/output/data_without_fdatasync.png',bbox_inches='tight', format='png', dpi=1000,pad_inches=0.0)