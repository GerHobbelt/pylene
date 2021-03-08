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
        with self.assertRaises(ValueError) as context:
            pln.id(arr)
        self.assertTrue("Array should be C contiguous" in str(context.exception))

    def test_from_numpy_checked(self):
        arr = np.arange(12).reshape((4, 3)).astype(np.int32)
        self.assertTrue(pln.check_from_numpy(arr))

    def test_ndbuffer_to_numpy(self):
        res = pln.iota()
        res2 = pln.id(res)
        self.assertTrue(res.dtype == res2.dtype == np.uint16)
        self.assertTrue(res.shape[0] == res2.shape[0] == 10)
        self.assertTrue(res.shape[1] == res2.shape[1] == 15)
        self.assertTrue(res.strides[0] == res2.strides[0] == 30)
        self.assertTrue(res.strides[1] == res2.strides[1] == 2)
        self.assertTrue(res.data == res2.data)

    def test_several_pylena_numpy_extension(self):
        """
        Just check there are no memory leaks or runtime error, not a precise result
        """
        import pylena_extension as ext
        img1 = pln.iota()
        img2 = ext.generate_iota()
        res1 = ext.id(img1)
        res2 = pln.id(img2)
        expected1 = np.arange(10 * 15).reshape((10, 15))
        expected2 = np.arange(20, 20 + 10 * 15).reshape((15, 10))
        self.assertTrue(np.all(res1 == expected1))
        self.assertTrue(np.all(res2 == expected2))

    def test_incorrect_type(self):
        ERROR_MSG = "Invalid dtype argument"

        img = np.zeros((10, 10), dtype=str)
        with self.assertRaises(ValueError) as context:
            pln.id(img)
            self.assertTrue(ERROR_MSG in str(context.exception))

        class WrongType:
            pass
        img = np.zeros((10, 10), dtype=WrongType)
        with self.assertRaises(ValueError) as context:
            pln.id(img)
            self.assertTrue(ERROR_MSG in str(context.exception))

    def test_memory(self):
        import gc
        def change_scope():
            img = pln.iota()
            img[0, 0] = 1303
            return pln.id(img)
        img = pln.iota()
        img2 = pln.id(img)
        img3 = change_scope()
        img[0, 0] = 1136
        self.assertTrue(img[0, 0] == img2[0, 0] == 1136)
        gc.collect()
        self.assertTrue(img3[0, 0] == 1303)
        del img
        gc.collect()
        self.assertTrue(img2[0, 0] == 1136)

    def test_returned_change_value(self):
        def by_reference(img):
            img[0, 0] = 1111

        def by_reference_returned(img):
            img[0, 0] = 2222
            return img

        img1 = pln.iota()
        img2 = pln.iota()
        img1[0, 0] = 3333
        img2[0, 0] = 4444
        by_reference(img1)
        img3 = by_reference_returned(img2)
        self.assertTrue(img1[0, 0] == 1111)
        self.assertTrue(img2[0, 0] == img3[0, 0] == 2222)

    def test_ref_count(self):
        import sys
        img = pln.iota()
        img2 = pln.id(img)
        self.assertTrue(sys.getrefcount(img.base) == sys.getrefcount(img2.base) == 3)
        base = img.base
        del img
        del img2
        self.assertTrue(sys.getrefcount(base) == 2)


if __name__ == "__main__":
    unittest.main()