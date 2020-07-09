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


radius = 3
sizes = {"width": 3138, "height": 3138}  # 10Mo
number = 100
percent = 20

sizes_list = [sizes]
radius_list = [r for r in range(0, 127, 2)]


def setup_test_img():
    global sizes
    ref = np.zeros((sizes["width"], sizes["height"]), dtype="uint8")
    rnd.seed(42)
    for x in range(0, sizes["width"]):
        for y in range(0, sizes["height"]):
            ref[x, y] = rnd.randint(0, 255)
    return ref


# def test_pylena():
#     disc = pln.se.disc(float(radius))
#     pln.morpho.dilate(ref, disc)

def test_pylena_decomp():
    disc = pln.se.disc_decomposable(float(radius))
    pln.morpho.dilate(ref, disc)


def test_sckimage():
    disc = skimorph.disk(radius, dtype="uint8")
    skimorph.dilation(ref, disc)


def test_cv2():
    disc = cv2.getStructuringElement(
        cv2.MORPH_ELLIPSE, (radius*2+1, radius*2+1))
    cv2.dilate(ref, disc, iterations=1)


def get_sizes():
    for s in sizes_list:
        yield s


def get_radius():
    for r in radius_list:
        yield r


def print_results(times):
    for t in times:
        print("Img {}x{} SE rad={} (nb={}): PlnD={}, Skimg={}, CV={}".format(
            t["sizes"]["width"], t["sizes"]["height"], t["radius"], t["number"],
            # t["Pylena"],
            t["PylenaD"], t["Skimage"], t["OpenCV"]
        ))


def save_result_to_csv(times, size):
    benchmarks = {
        # 'Pylena': [],
        'PylenaD': [],
        'Skimage': [],
        'OpenCV': []
    }

    for t in times:
        if t["sizes"]["width"] == size["width"] and t["sizes"]["height"] == size["height"]:
            # benchmarks['Pylena'].append(t["Pylena"])
            benchmarks['PylenaD'].append(t["PylenaD"])
            benchmarks['Skimage'].append(t["Skimage"])
            benchmarks['OpenCV'].append(t["OpenCV"])

    indexes = [r*2+1 for r in radius_list]
    df = pd.DataFrame(data=benchmarks, index=indexes, columns=[
                      'PylenaD', 'Skimage', 'OpenCV'])
    print(df)
    df.to_csv("bench_disc_by_SE.csv")


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
                    "radius": r,
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

    save_result_to_csv(times, {"width": 3138, "height": 3138})
