import unittest
import PyPylene
import numpy


class PyPyleneImage2d(unittest.TestCase):
    def test_from_buffer(self):
        arr = numpy.array([[1, 1, 1], [2, 2, 2], [3, 3, 3]], dtype="int8")
        img = PyPylene.image2d(arr)

    def test_stretch(self):
        arr = numpy.array([[1, 1, 1], [2, 2, 2], [3, 3, 3]], dtype="int8")
        img = PyPylene.image2d(arr)
        # image2d<float> needs to be bound for it to work
        # stretched_img = PyPylene.stretch(img)


if __name__ == "__main__":
    unittest.main()
