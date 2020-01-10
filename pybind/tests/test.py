import os
import sys
import trace
import numpy as np
import Pylena as pln



def test_from_buffer():
    ref = np.zeros((100, 100), dtype="uint8")
    ref[50, 50] = 255
    img = pln.ndimage(ref)
    ret = pln.morpho.dilate(img, 30)
    npret = np.array(ret)

    import matplotlib.pyplot as plt
    plt.imshow(npret)
    plt.show()


def main():
    test_from_buffer()


if __name__ == "__main__":
    main()
    # sys.stdout = sys.stderr
    # tracer = trace.Trace(trace=1, count=0, ignoredirs=["/usr", sys.prefix])
    # tracer.runfunc(main)
