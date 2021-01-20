import unittest
import numpy as np
from imageio import imread

import sys
sys.path.append("../../pylene")
import pylena as pln


class TestNumpyImage(unittest.TestCase):
    def _test_equal(self, arr, arr2):
        self.assertTrue(arr.ndim == arr2.ndim)
        for i in range(arr.ndim):
            self.assertTrue(arr.shape[i] == arr2.shape[i])
            self.assertTrue(arr.strides[i] == arr2.strides[i])
        self.assertTrue(arr.data == arr2.data)
        self.assertTrue(np.all(arr == arr2))

    def test_lena_grayscale(self):
        img = pln.io.imread(pln.utils.get_image_filename("lena.pgm"))
        ref = imread(pln.utils.get_image_filename("lena.pgm"))
        self.assertTrue(np.all(img == ref))

    def test_lena_color(self):
        img = pln.io.imread(pln.utils.get_image_filename("lena.ppm"))
        ref = imread(pln.utils.get_image_filename("lena.ppm"))
        self.assertTrue(np.all(img == ref))

    def test_id(self):
        arr = np.arange(12).reshape((3, 4))
        arr2 = pln.utils.id(arr)
        self._test_equal(arr, arr2)

        arr3 = pln.io.imread(pln.utils.get_image_filename("lena.pgm"))
        arr4 = pln.utils.id(arr3)
        self._test_equal(arr3, arr4)

    def test_id_fortran_contiguous(self):
        arr = np.arange(12).reshape((3, 4))
        arr2 = arr.T
        arr3 = pln.utils.id(arr2)
        self._test_equal(arr2, arr3)

if __name__ == "__main__":
    unittest.main()