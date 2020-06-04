import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import json
import re

sns.set()
sns.set_style("whitegrid")
sns.set_context("paper")

csv_lst = [
  "bench_pln_disc_by_SE",
  "bench_pln_rect_by_SE",
  "bench_disc_by_SE",
  "bench_rect_by_SE"
]

def draw_fig_from_csv(csv_file_name_without_ext):
  df = pd.read_csv(filepath_or_buffer=csv_file_name_without_ext + ".csv", index_col=0)

  fig = sns.lineplot(data=df, legend="full")
  fig.set(yscale="log", xlabel="SE size", ylabel="Time (ms)")

  fig.get_figure().savefig(csv_file_name_without_ext + ".png")
  fig.get_figure().clf()

if __name__ == "__main__":
  for f in csv_lst:
    draw_fig_from_csv(f)
