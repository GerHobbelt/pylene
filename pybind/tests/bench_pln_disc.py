import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import random as rnd
import timeit
import math

import Pylena as pln


radius = 3
sizes = {"width": 1000, "height": 1000}
number = 100
percent = 20


def setup_test_img():
    global sizes
    ref = np.zeros((sizes["width"], sizes["height"]), dtype="uint8")
    rnd.seed(42)
    nb_pixels = sizes["width"]*sizes["height"]
    nb_pixels_white = int(nb_pixels * percent / 100)
    for i in range(0, nb_pixels_white):
        x = rnd.randint(0, sizes["width"]-1)
        y = rnd.randint(0, sizes["height"]-1)
        ref[x, y] = 255
    return ref


def test_pylena():
    disc = pln.se.disc(float(radius))
    pln.morpho.dilate(ref, disc)


def test_pylena_decomp():
    disc = pln.se.disc_decomposable(float(radius))
    pln.morpho.dilate(ref, disc)


sizes_list = [sizes]
radius_list = [1+2**r for r in range(1, 8)]


def get_sizes():
    for s in sizes_list:
        yield s


def get_radius():
    for r in radius_list:
        yield r


def print_results(times):
    for t in times:
        print("Img {}x{} SE rad={} (nb={}): Pln={}, PlnD={}".format(
            t["sizes"]["width"], t["sizes"]["height"], t["radius"], t["number"],
            t["Pylena"], t["PylenaD"]
        ))


def plot_results_by_SE(times, size):
    x = radius_list
    y_pln = []
    y_plnD = []
    for t in times:
        if t["sizes"]["width"] == size["width"] and t["sizes"]["height"] == size["height"]:
            y_pln.append(t["Pylena"])
            y_plnD.append(t["PylenaD"])

    plt.figure("Benchmarks: Time vs. SE size")

    plt.plot(x, y_pln, label="Pylena")
    plt.plot(x, y_plnD, label="Pylena Decomp")

    plt.xlabel("SE radius size")
    plt.ylabel("Time (s)")
    plt.yscale("log")
    plt.title(
        "dilation ({}x{}) image, {} iterations".format(size["width"], size["height"], number))
    plt.legend()


def plot_results_by_pixels(times, radius):
    x = [x["width"] * x["height"] for x in sizes_list]
    y_pln = []
    y_plnD = []
    for t in times:
        if t["radius"] == radius:
            y_pln.append(t["Pylena"])
            y_plnD.append(t["PylenaD"])

    plt.figure("Benchmarks: Time vs. Image size")

    plt.plot(x, y_pln, label="Pylena")
    plt.plot(x, y_plnD, label="Pylena Decomp")

    plt.xlabel("Pixel number")
    plt.ylabel("Time (s)")
    plt.title(
        "dilation (rad={}) SE, {} iterations".format(radius, number))
    plt.legend()


def main_bench():
    global ref, sizes, radius

    times = []

    for s in get_sizes():
        sizes = s
        ref = setup_test_img()
        for r in get_radius():
            radius = r
            print("Benchmarking {} dilations of {}x{} uint8, disc radius={}...".format(
                number, s["width"], s["height"], r
            ))

            print("Starting Pylena...")
            time_pln = timeit.timeit(
                "test_pylena()",
                setup="from __main__ import test_pylena",
                number=number
            )
            print("End Pylena")

            print("Starting Pylena decomposable...")
            time_pln_decomp = timeit.timeit(
                "test_pylena_decomp()",
                setup="from __main__ import test_pylena_decomp",
                number=number
            )
            print("End Pylena decomposable")

            times.append(
                {
                    "number": number,
                    "sizes": s,
                    "radius": r,
                    "Pylena": time_pln,
                    "PylenaD": time_pln_decomp
                }
            )

    return times


if __name__ == "__main__":
    times = main_bench()
    print_results(times)

    plt.figure("Original image")
    plt.imshow(ref)

    plot_results_by_SE(times, {"width": 1000, "height": 1000})
    # plot_results_by_pixels(times, radius=15)

    plt.show()
