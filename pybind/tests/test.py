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


disc_radius = 15


def test_from_buffer():
    img = pln.ndimage(ref)
    disc = pln.se.disc(30.)
    ret = pln.morpho.dilate(img, disc)
    npret = np.array(ret)

    plt.imshow(npret)
    plt.show()


def test_pylena(ref):
    img = pln.ndimage(ref)
    disc = pln.se.disc(float(disc_radius))
    pln.morpho.dilate(img, disc)
    # ret = pln.morpho.dilate(img, disc)
    # npret = np.array(ret)
    # return npret


def test_sckimage(ref):
    disc = skimorph.disk(disc_radius, dtype="uint8")
    # skimorph.binary_dilation(ref, disc)
    ret = skimorph.binary_dilation(ref, disc)
    # return ret


def test_cv2(ref):
    disc = cv2.getStructuringElement(
        cv2.MORPH_ELLIPSE, (disc_radius*2, disc_radius*2))
    cv2.dilate(ref, disc, iterations=1)
    # ret = cv2.dilate(ref, disc, iterations=1)
    # return ret


def main_plot():
    ref = np.zeros((100, 100), dtype="uint8")
    ref[50, 50] = 255

    plt.figure("Pylena")
    ret_pln = test_pylena(ref)
    plt.imshow(ret_pln)

    plt.figure("Scikit-image")
    ret_skimg = test_sckimage(ref)
    plt.imshow(ret_skimg)

    plt.figure("OpenCV")
    ret_cv2 = test_cv2(ref)
    plt.imshow(ret_cv2)

    plt.show()


size = {"width": 1000, "height": 1000}


def setup_test_img():
    ref = np.zeros((size["width"], size["height"]), dtype="uint8")
    rnd.seed(42)
    nb_pixels = size["width"]*size["height"]
    nb_pixels_white = int(
        nb_pixels * 5 / math.pow(10, int(math.log10(nb_pixels))))
    for i in range(0, nb_pixels_white):
        x = rnd.randint(0, size["width"]-1)
        y = rnd.randint(0, size["height"]-1)
        ref[x, y] = 255
    return ref


def main_bench():
    number = 10
    print("{} dilations of {}x{} uint8, SE disc radius: {}".format(
        number, size["width"], size["height"], disc_radius))

    print("Pylena: {}".format(timeit.timeit("test_pylena(setup_test_img())",
                                            setup="from __main__ import test_pylena, setup_test_img", number=number)))
    print("Scikit-image: {}".format(timeit.timeit("test_sckimage(setup_test_img())",
                                                  setup="from __main__ import test_sckimage, setup_test_img", number=number)))
    print("OpenCV: {}".format(timeit.timeit("test_cv2(setup_test_img())",
                                            setup="from __main__ import test_cv2, setup_test_img", number=number)))


if __name__ == "__main__":
    # main_plot()
    main_bench()
