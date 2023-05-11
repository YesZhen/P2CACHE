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

fig, ax = plt.subplots(1, 3, figsize = (20,4))
df0 = pd.read_csv("share/output/filebench.csv", delimiter = "\t")
df1 = pd.read_csv("share/output/rocksdb.csv", delimiter = "\t")
df2 = pd.read_csv("share/output/minio.csv", delimiter = "\t")

data_names = ["Ext4", "Ext4-DAX", "Ext4-DJ", "NOVA",
        "p2Cache", "XFS", "XFS-DAX"]

label_names = ["Ext4", "Ext4-DAX", "Ext4-DJ", "NOVA",
        "p2Cache", "XFS", "XFS-DAX"]

x = np.arange(len(df0["Ext4"]), dtype = float)

for idx, i in enumerate(data_names):
    offset, wid = calc_pos(bar_num, idx)
    ax[0].bar(x + offset, df0[i] / 1000000, label = label_names[idx], color = colors[idx], width = wid, edgecolor='black', hatch = patterns[idx])
ax[0].set_yscale('log')
ax[0].grid(axis= 'y',linestyle='--')
ax[0].tick_params(axis='both', labelsize=16)
ax[0].set_ylabel("Operations per second\n(Mops/Sec)", fontsize = 16)
ax[0].set_xticks(x, ["Fileserver", "Webproxy", "Varmail"])
ax[0].set_yticks([0.03, 0.05, 0.1, 0.2, 0.4, 1], ["0.03", "0.05", "0.1", "0.2", "0.4", "1"])
ax[0].set_title("(a) Filebench", fontsize = 16, y= -0.2, x = 0.5)

x = np.arange(len(df1["Ext4"]), dtype = float)
for idx, i in enumerate(data_names):
    offset, wid = calc_pos(bar_num, idx)
    ax[1].bar(x + offset, df1[i] / 1000000, color = colors[idx], width = wid, edgecolor='black', hatch = patterns[idx])
ax[1].set_yscale('log')
ax[1].grid(axis= 'y',linestyle='--')
ax[1].tick_params(axis='both', labelsize=16)
ax[1].set_xticks(x, ["Insert(100B)", "Insert(1KB)", "Read(100B)", "Read(1KB)"])
ax[1].set_yticks([0.001, 0.01, 0.1, 0.2, 0.4, 1, 2], ["0.001", "0.01", "0.1", "0.2", "0.4", "1", "2"])
ax[1].set_title("(b) RocksDB", fontsize = 16, y= -0.2, x = 0.5)
    
x = np.arange(len(df2["Ext4"]), dtype = float)
for idx, i in enumerate(data_names):
    offset, wid = calc_pos(bar_num, idx)
    ax[2].bar(x + offset, df2[i] / 1000000, color = colors[idx], width = wid, edgecolor='black', hatch = patterns[idx])
ax[2].set_yscale('log')
ax[2].grid(axis= 'y',linestyle='--')
ax[2].tick_params(axis='both', labelsize=16)
ax[2].set_xticks(x, ["Put", "Get", "Mixed"])
ax[2].set_yticks([0.0004, 0.001, 0.002, 0.003 , 0.005], ["0.0004", "0.001", "0.002", "0.003", "0.005"])
ax[2].set_title("(c) MinIO", fontsize = 16, y= -0.2, x = 0.5)

fig.legend(ncol = 7, fontsize = 16, frameon=False, bbox_to_anchor=(0.85, 1.1))

fig.tight_layout(pad =0)
foo_fig = plt.gcf() # 'get current figure'
foo_fig.savefig('fig/10.pdf',bbox_inches='tight', format='pdf', dpi=1000,pad_inches=0.0)