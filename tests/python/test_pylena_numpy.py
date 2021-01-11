import unittest
import numpy as np
from imageio import imread

import sys
sys.path.append("../../pylene")
import pylena as pln


class TestNumpyImage(unittest.TestCase):
    def test_lena_grayscale(self):
        img = pln.io.imread(pln.utils.get_image_filename("lena.pgm"))
        ref = imread(pln.utils.get_image_filename("lena.pgm"))
        self.assertTrue(np.all(img == ref))

    def test_lena_color(self):
        img = pln.io.imread(pln.utils.get_image_filename("lena.ppm"))
        ref = imread(pln.utils.get_image_filename("lena.ppm"))
        self.assertTrue(np.all(img == ref))

if __name__ == "__main__":
    unittest.main()