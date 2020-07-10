import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import json
import re
import math

sns.set()
sns.set_style("whitegrid")
sns.set_context("paper")

csv_lst = [
    #  "bench_pln_disc_by_SE",
    #  "bench_pln_rect_by_SE",
    "bench_disc_by_SE",
    "bench_rect_by_SE"
]


def draw_fig_from_csv(csv_rect_file_name_without_ext, csv_disc_file_name_without_ext):
    df_disc = pd.read_csv(
        filepath_or_buffer=csv_disc_file_name_without_ext + ".csv", index_col=0)
    df_disc.index = [int((r-1)/2) for r in df_disc.index]
    df_disc.columns = ["Disc-" + c for c in df_disc.columns]

    df_rect = pd.read_csv(
        filepath_or_buffer=csv_rect_file_name_without_ext + ".csv", index_col=0)
    df_rect.index = [int((r-1)/2) for r in df_rect.index]
    df_rect.columns = ["Rect-" + c for c in df_rect.columns]

    df = df_disc.append(df_rect)

    fig = sns.lineplot(data=df, legend="full")
    fig.set(yscale="log", xlabel="SE size (half edge size for square, radius for disc)",
            ylabel="Time (ms)")

    fig.get_figure().savefig("bench_disc_rect_by_SE.pdf")
    fig.get_figure().clf()


if __name__ == "__main__":
    draw_fig_from_csv(csv_lst[1], csv_lst[0])
