#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import os
import pandas as pd

patterns = [ "//" , "\\\\" , "||||" , "++++" , "//-", "\\\\-" , "||-", "++\\", "O", "." ]

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

mks= ["o", "v", "^", "<", "*", "x", "p", "s", "D", "P"]

df = pd.read_csv("share/output/scalability.csv", delimiter = ",", encoding='utf8')
fig, ax = plt.subplots(figsize = (9, 4))
names = ["ext4_ops", "ext4_dax_ops", "ext4_dj_ops", "nova_ops", 
         "p2cache_ops", "xfs_ops", "xfs_dax_ops"]

x = np.arange(len(df["ext4_ops"]))
for idx, i in enumerate(names):
    ax.plot(x , df[i] / 1000, label = names[idx], color = colors[idx], marker=mks[idx], markeredgecolor = '#1C1C1C', markersize = 10)
ax.set_yscale('log')
fig.legend(ncol = 4, fontsize = 16, frameon=False, bbox_to_anchor=(0.9, 1.1))

ax.set_xticks(range(7), ["1", "2", "4", "6", "8", "10", "12"])
ax.set_yticks([1, 10, 100, 1000], ["1", "10", "100", "1000"])
ax.tick_params(axis='both', labelsize=16)
ax.set_ylabel("IOPS(*1000)", fontsize = 16)
ax.set_xlabel("Thread #", fontsize = 16)
ax.xaxis.set_label_coords(0.5, .1)
ax.grid(axis= 'y',linestyle='--')
# fig.tight_layout(pad =0)
foo_fig = plt.gcf() # 'get current figure'
foo_fig.savefig('share/output/scalability.png',bbox_inches='tight', format='png', dpi=1000,pad_inches=0.0)