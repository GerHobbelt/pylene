#! /usr/bin/python

import json
import sys
import matplotlib.pyplot as plt
import pandas as pd
from mpltools import style

style.use('ggplot')
df = {}
for f in sys.argv[1:]:
    platform = f.split(".")[-2]
    data = json.load( open(f) )
    data = data["benchmarks"]

    bench_name = ""
    names = []
    times_avg = []
    times_stdv = []
    for bench in data:
        if bench["name"].endswith("_mean"):
            bench_name, name = bench["name"].split("/")
            names.append(name[:-5])
            times_avg.append(bench["real_time"] / 1000000)
        elif bench["name"].endswith("_stddev"):
            bench_name, name = bench["name"].split("/")
            times_stdv.append(bench["real_time"] / 1000000)
    df[platform] = { n : v for n,v,err in zip(names, times_avg, times_stdv) }

df = pd.DataFrame.from_dict(df)
print df
ax = df.plot(kind="barh")

for p in ax.patches:
    ax.annotate("%i ms " % p.get_width(),
                (p.get_width(), p.get_y() + p.get_height()/2),
                horizontalalignment="right", verticalalignment="center")

'''
n = len(names)
plt.barh(range(n), times_avg, align="center", xerr = times_stdv, label=times_avg)
plt.yticks(range(n), names)
'''

plt.xlabel("Performance (ms)")
plt.title(bench_name)
plt.show()

