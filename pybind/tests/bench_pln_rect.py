import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import random as rnd
import timeit
import math

import Pylena as pln


rect_width = 15
rect_height = 15
sizes = {"width": 50, "height": 50}
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

ref = setup_test_img()


def test_pylena():
    img = pln.ndimage(ref)
    rect = pln.se.rect2d(rect_width, rect_height)
    pln.morpho.dilate(img, rect)

def test_pylena_decomp():
    img = pln.ndimage(ref)
    rect = pln.se.rect2d_decomposable(rect_width, rect_height)
    pln.morpho.dilate(img, rect)


sizes_list = [sizes] + \
    [{"width": x*100, "height": x*100} for x in range(1, 11)]
rect_sizes_list = [{"width": x, "height": x} for x in range(3, 16)]


def get_sizes():
    for s in sizes_list:
        yield s

def get_rect_sizes():
    for x in rect_sizes_list:
        yield x


def print_results(times):
    for t in times:
        print("Img {}x{} SE rect={}x{} (nb={}): Pln={}, PlnD={}".format(
            t["sizes"]["width"], t["sizes"]["height"], t["rect"]["width"], t["rect"]["height"],
            t["number"], t["Pylena"], t["PylenaD"]
        ))

def plot_results(times, size):
    x = [x["width"] for x in rect_sizes_list]
    y_pln = []
    y_plnD = []
    for t in times:
        if t["sizes"]["width"] == size["width"] and t["sizes"]["height"] == size["height"]:
            y_pln.append(t["Pylena"])
            y_plnD.append(t["PylenaD"])

    plt.figure("Original image")
    plt.imshow(ref)

    plt.figure("Benchmarks")

    plt.plot(x, y_pln, label="Pylena")
    plt.plot(x, y_plnD, label="Pylena Decomp")

    plt.xlabel("SE square edge size")
    plt.ylabel("Time (s)")
    plt.title(
        "dilation ({}x{}) image, {} iterations".format(size["width"], size["height"], number))
    plt.legend()
    plt.show()


def main_bench():
    global rect_width, rect_height, sizes, radius

    times = []

    for s in get_sizes():
        sizes = s
        ref = setup_test_img()
        for rect in get_rect_sizes():
            rect_width = rect["width"]
            rect_height = rect["height"]
            print("Benching {} dilations of {}x{} uint8, rect={}x{}...".format(
                number, s["width"], s["height"], rect["width"], rect["height"]
            ))

            times.append(
                {
                    "number": number,
                    "sizes": s,
                    "rect": rect,
                    "Pylena": timeit.timeit(
                        "test_pylena()",
                        setup="from __main__ import test_pylena",
                        number=number
                    ),
                    "PylenaD": timeit.timeit(
                        "test_pylena_decomp()",
                        setup="from __main__ import test_pylena_decomp",
                        number=number
                    )
                }
            )

    return times


if __name__ == "__main__":
    times = main_bench()
    print_results(times)
    plot_results(times, {"width": 1000, "height": 1000})
