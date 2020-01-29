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


rect_width = 3
rect_height = 3
sizes = {"width": 3138, "height": 3138}  # 10Mo
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


# def test_pylena():
#     rect = pln.se.rect2d(rect_width, rect_height)
#     pln.morpho.dilate(ref, rect)

def test_pylena_decomp():
    rect = pln.se.rect2d_decomposable(rect_width, rect_height)
    pln.morpho.dilate(ref, rect)


def test_sckimage():
    rect = skimorph.rectangle(rect_width, rect_height, dtype="uint8")
    skimorph.binary_dilation(ref, rect)


def test_cv2():
    rect = cv2.getStructuringElement(
        cv2.MORPH_RECT, (rect_width, rect_height))
    cv2.dilate(ref, rect, iterations=1)


sizes_list = [sizes]
rect_sizes_list = [{"width": 1+2**x, "height": 1+2**x} for x in range(1, 8)]


def get_sizes():
    for s in sizes_list:
        yield s


def get_rect_sizes():
    for x in rect_sizes_list:
        yield x


def print_results(times):
    for t in times:
        print("Img {}x{} SE rect={}x{} (nb={}): PlnD={}, Skimg={}, CV={}".format(
            t["sizes"]["width"], t["sizes"]["height"], t["rect"]["width"], t["rect"]["height"],
            t["number"],
            # t["Pylena"],
            t["PylenaD"], t["Skimage"], t["OpenCV"]
        ))


def plot_results_by_SE(times, size):
    x = [x["width"] for x in rect_sizes_list]
    # y_pln = []
    y_plnD = []
    y_skimage = []
    y_cv = []
    for t in times:
        if t["sizes"]["width"] == size["width"] and t["sizes"]["height"] == size["height"]:
            # y_pln.append(t["Pylena"])
            y_plnD.append(t["PylenaD"])
            y_skimage.append(t["Skimage"])
            y_cv.append(t["OpenCV"])

    plt.figure("Benchmarks: Time vs. SE size")

    # plt.plot(x, y_pln, label="Pylena")
    plt.plot(x, y_plnD, label="Pylena Decomp")
    plt.plot(x, y_skimage, label="Skimage")
    plt.plot(x, y_cv, label="OpenCV")

    plt.xlabel("SE square edge size")
    plt.ylabel("Time (s)")
    plt.yscale("log")
    plt.title(
        "dilation ({}x{}) image, {} iterations".format(size["width"], size["height"], number))
    plt.legend()


def plot_results_by_pixels(times, rect_width, rect_height):
    x = [x["width"] * x["height"] for x in sizes_list]
    # y_pln = []
    y_plnD = []
    y_skimage = []
    y_cv = []
    for t in times:
        if t["rect"]["width"] == rect_width and t["rect"]["height"] == rect_height:
            # y_pln.append(t["Pylena"])
            y_plnD.append(t["PylenaD"])
            y_skimage.append(t["Skimage"])
            y_cv.append(t["OpenCV"])

    plt.figure("Benchmarks: Time vs. Image size")

    # plt.plot(x, y_pln, label="Pylena")
    plt.plot(x, y_plnD, label="Pylena Decomp")
    plt.plot(x, y_skimage, label="Skimage")
    plt.plot(x, y_cv, label="OpenCV")

    plt.xlabel("Pixel number")
    plt.ylabel("Time (s)")
    plt.title(
        "dilation ({}x{}) SE, {} iterations".format(rect_width, rect_width, number))
    plt.legend()


def main_bench():
    global rect_width, rect_height, sizes, radius, ref

    times = []

    for s in get_sizes():
        sizes = s
        ref = setup_test_img()
        for rect in get_rect_sizes():
            rect_width = rect["width"]
            rect_height = rect["height"]
            print("Benchmarking {} dilations of {}x{} uint8, rect={}x{}...".format(
                number, s["width"], s["height"], rect["width"], rect["height"]
            ))

            # print("Starting Pylena...")
            # time_pln = timeit.timeit(
            #     "test_pylena()",
            #     setup="from __main__ import test_pylena",
            #     number=number
            # )
            # print("End Pylena")

            print("Starting Pylena decomposable...")
            time_pln_decomp = timeit.timeit(
                "test_pylena_decomp()",
                setup="from __main__ import test_pylena_decomp",
                number=number
            )
            print("End Pylena decomposable")

            print("Starting scikit-image...")
            time_skimage = timeit.timeit(
                "test_sckimage()",
                setup="from __main__ import test_sckimage",
                number=number
            )
            print("End scikit-image")

            print("Starting OpenCV...")
            time_cv2 = timeit.timeit(
                "test_cv2()",
                setup="from __main__ import test_cv2",
                number=number
            )
            print("End OpenCV")

            times.append(
                {
                    "number": number,
                    "sizes": s,
                    "rect": rect,
                    # "Pylena": time_pln,
                    "PylenaD": time_pln_decomp,
                    "Skimage": time_skimage,
                    "OpenCV": time_cv2
                }
            )

    return times


if __name__ == "__main__":
    times = main_bench()
    print_results(times)

    plt.figure("Original image")
    plt.imshow(ref)

    plot_results_by_SE(times, {"width": 3138, "height": 3138})
    # plot_results_by_pixels(times, rect_width=15, rect_height=15)

    plt.show()
