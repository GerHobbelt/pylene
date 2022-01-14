import numpy as np
import skimage
import timeit
import pylena as pln
from ValueSetExample import MyValueSet, MyStruct

grayscale = skimage.data.camera()

vs = MyValueSet()


def test_fast():
    global grayscale
    pln_ret = pln.morpho.stretch(grayscale)
    return pln_ret


def test_slow():
    global grayscale, vs
    pln_ret = pln.morpho.stretch(grayscale, vs)
    return pln_ret


def test_virtual_dispatch():
    global grayscale
    pln_ret = pln.morpho.stretch_virtual_dispatch(grayscale)
    return pln_ret


def test_virtual_dispatch_type_erased_value():
    global grayscale
    pln_ret = pln.morpho.stretch_virtual_dispatch_type_erased_value(grayscale)
    return pln_ret


if __name__ == "__main__":
    print("Benchmarking static-dynamic bridge...")
    cpp_fast_ret = timeit.timeit("test_fast()", globals=locals(), number=10)
    cpp_virtual_dispatch_ret = timeit.timeit(
        "test_virtual_dispatch()", globals=locals(), number=10)
    cpp_virtual_dispatch_type_erased_value_ret = timeit.timeit(
        "test_virtual_dispatch_type_erased_value()", globals=locals(), number=10)
    cpp_python_vs_ret = timeit.timeit(
        "test_slow()", globals=locals(), number=10)
    print("Benchmarking done...")

    print(
        "Native value-set with native C++ value-type: {0:.4f}sec".format(cpp_fast_ret))
    print(
        "Value-set with virtual dispatch with native C++ value-type: {0:.4f}sec".format(cpp_virtual_dispatch_ret))
    print(
        "Value-set with virtual dispatch with C++ type-erased values: {0:.4f}sec".format(cpp_virtual_dispatch_type_erased_value_ret))
    print(
        "Injected Python value-set with native C++ value-type: {0:.4f}sec".format(cpp_python_vs_ret))
