#! /usr/bin/python3
import json
import sys
import matplotlib.pyplot as plt
import pandas as pd


def plotbysize(filename):
    data = json.load(open(filename))
    entries = data["benchmarks"]

    for entry in entries:
        _, algo, size = entry["name"].split("/")
        entry["algo"] = algo
        entry["size"] = int(size)
        entry["bytes_per_second"] = entry["bytes_per_second"] / (2 ** 20)
        del entry["name"]
        del entry["cpu_time"]
        del entry["iterations"]
        del entry["real_time"]
        del entry["time_unit"]

    df = pd.DataFrame(entries)


    df.set_index(["size", "algo"], inplace=True)
    df = df.unstack()["bytes_per_second"]
    df.plot(logy=True)


    plt.xlabel("SE Size")
    plt.ylabel("Performance (MB/s)")
    plt.show()
