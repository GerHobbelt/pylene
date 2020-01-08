import os, sys, trace
import numpy as np
import unittest as ut

sys.path.append(os.getcwd())

import Pylena as pln


class PylenaImage(ut.TestCase):
    def test_from_buffer(self):
        ref = np.array([[1, 1, 1], [2, 2, 2], [3, 3, 3]], dtype="int8")
        img = pln.Image(ref)
        # arr = pln.to_numpy(img)
        # self.assertEqual(ref.all(), arr.all())

    # def test_stretch(self):
    #     arr = np.array([[1, 1, 1], [2, 2, 2], [3, 3, 3]], dtype="int8")
    #     img = pln.image2d(arr)
    #     stretched_img = pln.stretch(img)
    #     stretched_arr = pln.to_numpy(stretched_img)
    #     ref = arr / 127
    #     self.assertEqual(ref.all(), stretched_arr.all())

def main():
    unittest.main()

if __name__ == "__main__":
    sys.stdout = sys.stderr
    tracer = trace.Trace(trace=1, count=0, ignoredirs=["/usr", sys.prefix])
    tracer.runfunc(main)
