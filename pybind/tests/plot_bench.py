import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import json, re

sns.set()
sns.set_style("whitegrid")
sns.set_context("paper")


bench_result = json.load(open("../../build/bench/BMPlnVsOpenCV_Dilation_result.json"))

benchmarks = {'Pln_DiscDecompPeriodic': [], 'Pln_RectDecomp': [], 'CV_Disc': [], 'CV_Rect': []}
size_se_p = re.compile(".*/([0-9]+)")
indexes = []
for bench in bench_result['benchmarks']:
    for k in benchmarks.keys():
        if k in bench['name']:
            size_se = size_se_p.search(bench['name'])
            assert(size_se is not None)
            benchmarks[k].append({
                'size_se': int(size_se.group(1)),
                'iterations': bench['iterations'],
                'time': bench['real_time'],
                'unit': bench['time_unit'],
                'throughput_bps': bench['bytes_per_second']
            })

data = {'Pln_DiscDecompPeriodic': [], 'Pln_RectDecomp': [], 'CV_Disc': [], 'CV_Rect': []}
indexes = []

for k in benchmarks.keys():
    for ret in benchmarks[k]:
        data[k].append(ret['time'])

_k = list(benchmarks.keys())[0]
for ret in benchmarks[_k]:
    indexes.append(ret['size_se'])
    
df = pd.DataFrame(data=data, index=indexes, columns=['Pln_DiscDecompPeriodic', 'Pln_RectDecomp', 'CV_Disc', 'CV_Rect'])

fig = sns.lineplot(data=df, legend="full")
fig.set(yscale="log", xlabel="SE size", ylabel="Time (ns)")

grid.get_figure().savefig("PlnVsOpenCV.png")
