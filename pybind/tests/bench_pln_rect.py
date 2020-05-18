import cv2
import skimage.morphology as skimorph
import Pylena as pln
import pandas as pd
import os
import sys
import numpy as np
import random as rnd
import timeit
import math


rect_width = 3
rect_height = 3
sizes = {"width": 1000, "height": 1000}  # 10Mo
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
    rect = pln.se.rect2d(rect_width, rect_height)
    pln.morpho.dilate(ref, rect)


def test_pylena_decomp():
    rect = pln.se.rect2d_decomposable(rect_width, rect_height)
    pln.morpho.dilate(ref, rect)


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
        print("Img {}x{} SE rect={}x{} (nb={}): Pln={}, PlnD={}".format(
            t["sizes"]["width"], t["sizes"]["height"], t["rect"]["width"], t["rect"]["height"],
            t["number"], t["Pylena"], t["PylenaD"]
        ))


def save_result_to_csv(times, size):
    benchmarks = {
        'Pylena': [],
        'PylenaD': [],
    }

    for t in times:
        if t["sizes"]["width"] == size["width"] and t["sizes"]["height"] == size["height"]:
            benchmarks['Pylena'].append(t["Pylena"])
            benchmarks['PylenaD'].append(t["PylenaD"])

    indexes = [x['height'] * x['width'] for x in rect_sizes_list]
    df = pd.DataFrame(data=benchmarks, index=indexes, columns=[
                      'Pylena', 'PylenaD'])
    print(df)
    df.to_csv("bench_pln_rect_by_SE.csv")


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
                    "rect": rect,
                    "Pylena": time_pln,
                    "PylenaD": time_pln_decomp
                }
            )

    return times


if __name__ == "__main__":
    times = main_bench()
    print_results(times)

    save_result_to_csv(times, {"width": 1000, "height": 1000})
