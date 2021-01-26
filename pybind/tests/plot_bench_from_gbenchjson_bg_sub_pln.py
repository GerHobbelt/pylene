import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import json
import re

sns.set()
# sns.set_style("whitegrid")
sns.set_context("paper")


bench_result = json.load(open("BMPlnVsOpenCV_BgSubPipeline-GNU-9.3.0.json"))


def draw_fig(img_id):
    global bench_result

    benchmarks = {'Pln_PipeViewsRect': [], 'Pln_PipeAlgosRect': [],
                  'Pln_PipeViewsDisc': [], 'Pln_PipeAlgosDisc': []}

    pattern = ".*/[0-9]+/([0-9]+)"
    size_se_p = re.compile(pattern)
    indexes = []
    for bench in bench_result['benchmarks']:
        for k in benchmarks.keys():
            if k in bench['name']:
                if 'aggregate_name' in bench:
                    continue
                size_se = size_se_p.search(bench['name'])
                assert(size_se is not None)
                benchmarks[k].append({
                    'size_se': int(size_se.group(1)),
                    'iterations': bench['iterations'],
                    'time': bench['real_time'],
                    'unit': bench['time_unit'],
                    'throughput_bps': bench['bytes_per_second']
                })

    data = {'Pln_PipeViewsRect': [], 'Pln_PipeAlgosRect': [],
            'Pln_PipeViewsDisc': [], 'Pln_PipeAlgosDisc': []}
    indexes = []

    for k in benchmarks.keys():
        for ret in benchmarks[k]:
            data[k].append(ret['time'])

    _k = list(benchmarks.keys())[0]
    for ret in benchmarks[_k]:
        indexes.append(ret['size_se'])

    df = pd.DataFrame(data=data, index=indexes, columns=[
        'Pln_PipeViewsRect', 'Pln_PipeAlgosRect',
        'Pln_PipeViewsDisc', 'Pln_PipeAlgosDisc'])

    df.index.name = "SE size (half edge size for square, radius for disc)"

    print(df.describe())

    fig = sns.lineplot(data=df, hue="SE size", legend="full")
    fig.set(yscale="log", ylabel="Time (ms)")

    fig_name = "PlnVsOpenCV_bg_sub_pln_{}.pdf".format(img_id)
    fig.get_figure().savefig(fig_name)

    fig.clear()


if __name__ == '__main__':
    for i in range(0, 12):
        draw_fig(i)
