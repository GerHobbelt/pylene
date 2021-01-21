import sys
sys.path.append("../../pylene-python")

import unittest
import numpy as np

import pylena_test as pln


class TestNumpyImage(unittest.TestCase):
    def test_from_numpy_uint8(self):
        arr = np.array([[0, 0, 0, 1],
                        [0, 1, 0, 1],
                        [0, 0, 0, 1]]).astype(np.uint8)
        res = pln.id(arr)
        self.assertTrue(arr.ndim == res.ndim == 2)
        self.assertTrue(arr.shape[0] == res.shape[0] == 3)
        self.assertTrue(arr.shape[1] == res.shape[1] == 4)
        self.assertTrue(arr.strides[0] == res.strides[0] == 4)
        self.assertTrue(arr.strides[1] == res.strides[1] == 1)
        self.assertTrue(arr.dtype == res.dtype == np.uint8)
        self.assertTrue(arr.data == res.data)

    def test_from_numpy_uint64(self):
        import platform
        if not platform.architecture()[0] == '64bit':
            return
        arr = np.array([[0, 0, 0, 1],
                        [0, 1, 0, 1],
                        [0, 0, 0, 1]]).astype(np.uint64)
        res = pln.id(arr)
        self.assertTrue(arr.ndim == res.ndim == 2)
        self.assertTrue(arr.shape[0] == res.shape[0] == 3)
        self.assertTrue(arr.shape[1] == res.shape[1] == 4)
        self.assertTrue(arr.strides[0] == res.strides[0] == 32)
        self.assertTrue(arr.strides[1] == res.strides[1] == 8)
        self.assertTrue(arr.dtype == res.dtype == np.uint64)
        self.assertTrue(arr.data == res.data)

    def test_from_numpy_fortran_order(self):
        arr = np.array([[0, 1, 2],
                        [3, 4, 5],
                        [6, 7, 8],
                        [9, 10, 11]], order="F").astype(np.uint8)
        res = pln.id(arr)
        self.assertTrue(arr.ndim == res.ndim == 2)
        self.assertTrue(arr.shape[0] == res.shape[0] == 4)
        self.assertTrue(arr.shape[1] == res.shape[1] == 3)
        self.assertTrue(arr.strides[0] == res.strides[0] == 1)
        self.assertTrue(arr.strides[1] == res.strides[1] == 4)
        self.assertTrue(arr.dtype == res.dtype == np.uint8)
        self.assertTrue(arr.data == res.data)

    def test_from_numpy_checked(self):
        arr = np.arange(12).reshape((4, 3)).astype(np.int32)
        self.assertTrue(pln.check_from_numpy(arr))

    # Fail because of ndimage point access optimization (ndbuffer_image_impl<T, pdim>::get_pointer)
    #def test_from_numpy_f_checked(self):
    #    arr = np.asfortranarray(np.arange(12).reshape((4, 3)).astype(np.int32))
    #    self.assertTrue(pln.check_from_numpy_f(arr))

if __name__ == "__main__":
    unittest.main()