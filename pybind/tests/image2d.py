import unittest
import numpy
import PyPylene as pln


class PyPyleneImage2d(unittest.TestCase):
    def test_from_buffer(self):
        ref = numpy.array([[1, 1, 1], [2, 2, 2], [3, 3, 3]], dtype="int8")
        img = pln.image2d(ref)
        arr = pln.to_numpy(img)
        self.assertEqual(ref.all(), arr.all())

    def test_stretch(self):
        arr = numpy.array([[1, 1, 1], [2, 2, 2], [3, 3, 3]], dtype="int8")
        img = pln.image2d(arr)
        stretched_img = pln.stretch(img)
        stretched_arr = pln.to_numpy(stretched_img)
        ref = arr / 127
        self.assertEqual(ref.all(), stretched_arr.all())


if __name__ == "__main__":
    unittest.main()
