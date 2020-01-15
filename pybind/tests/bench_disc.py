import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import random as rnd
import timeit
import math

import Pylena as pln
import skimage.morphology as skimorph
import cv2


radius = 3
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
    disc = pln.se.disc(float(radius))
    pln.morpho.dilate(img, disc)

def test_pylena_decomp():
    img = pln.ndimage(ref)
    disc = pln.se.disc_decomposable(float(radius))
    pln.morpho.dilate(img, disc)

def test_sckimage():
    disc = skimorph.disk(radius, dtype="uint8")
    skimorph.binary_dilation(ref, disc)

def test_cv2():
    disc = cv2.getStructuringElement(
        cv2.MORPH_ELLIPSE, (radius*2, radius*2))
    cv2.dilate(ref, disc, iterations=1)


sizes_list = [sizes] + \
    [{"width": x*100, "height": x*100} for x in range(1, 11)]
radius_list = [r for r in range(3, 16)]


def get_sizes():
    for s in sizes_list:
        yield s

def get_radius():
    for r in radius_list:
        yield r


def print_results(times):
    for t in times:
        print("Img {}x{} SE rad={} (nb={}): Pln={}, PlnD={}, Skimg={}, CV={}".format(
            t["sizes"]["width"], t["sizes"]["height"], t["radius"], t["number"], t["Pylena"], t["PylenaD"], t["Skimage"], t["OpenCV"]
        ))

def plot_results_by_SE(times, size):
    x = radius_list
    y_pln = []
    y_plnD = []
    y_skimage = []
    y_cv = []
    for t in times:
        if t["sizes"]["width"] == size["width"] and t["sizes"]["height"] == size["height"]:
            y_pln.append(t["Pylena"])
            y_plnD.append(t["PylenaD"])
            y_skimage.append(t["Skimage"])
            y_cv.append(t["OpenCV"])

    plt.figure("Benchmarks: Time vs. SE size")

    plt.plot(x, y_pln, label="Pylena")
    plt.plot(x, y_plnD, label="Pylena Decomp")
    plt.plot(x, y_skimage, label="Skimage")
    plt.plot(x, y_cv, label="OpenCV")

    plt.xlabel("SE radius size")
    plt.ylabel("Time (s)")
    plt.title(
        "dilation ({}x{}) image, {} iterations".format(size["width"], size["height"], number))
    plt.legend()

def plot_results_by_pixels(times, radius):
    x = [x["width"] * x["height"] for x in sizes_list]
    y_pln = []
    y_plnD = []
    y_skimage = []
    y_cv = []
    for t in times:
        if t["radius"] == radius:
            y_pln.append(t["Pylena"])
            y_plnD.append(t["PylenaD"])
            y_skimage.append(t["Skimage"])
            y_cv.append(t["OpenCV"])

    plt.figure("Benchmarks: Time vs. Image size")

    plt.plot(x, y_pln, label="Pylena")
    plt.plot(x, y_plnD, label="Pylena Decomp")
    plt.plot(x, y_skimage, label="Skimage")
    plt.plot(x, y_cv, label="OpenCV")

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

            times.append(
                {
                    "number": number,
                    "sizes": s,
                    "radius": r,
                    "Pylena": timeit.timeit(
                        "test_pylena()",
                        setup="from __main__ import test_pylena",
                        number=number
                    ),
                    "PylenaD": timeit.timeit(
                        "test_pylena_decomp()",
                        setup="from __main__ import test_pylena_decomp",
                        number=number
                    ),
                    "Skimage":
                    timeit.timeit(
                        "test_sckimage()",
                        setup="from __main__ import test_sckimage",
                        number=number
                    ),
                    "OpenCV": timeit.timeit(
                        "test_cv2()",
                        setup="from __main__ import test_cv2",
                        number=number
                    )
                }
            )

    return times


if __name__ == "__main__":
    times = main_bench()
    print_results(times)

    plt.figure("Original image")
    plt.imshow(ref)

    plot_results_by_SE(times, {"width": 1000, "height": 1000})
    plot_results_by_pixels(times, radius=15)

    plt.show()
