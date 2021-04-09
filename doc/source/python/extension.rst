Writing an extension module
===========================

While writing an algorithm in C++ is great for performance issue, it is not easy
to prototype algorithms in this language. To solve this problem, Pylene provides
a library, ``Pylene-numpy``, which makes the automatic conversion between
Pylene images and Numpy arrays, thanks to the Pybind11's ``type_caster``.

In the following, it is explained how to make use of this library to create an
extension module.

Step 1: Setup the project
--------------------------

At first, a ``conanfile.txt`` is created.

.. code-block:: text

    [generators]
    cmake_find_package
    cmake

    [requires]
    pylene/head@lrde/unstable
    pybind11/2.6.2

    [options]
    pylene:fPIC=True # or pylene:shared=True

In this conanfile, two Conan's generators are used: the ``cmake`` and
the ``cmake_find_package`` generators. In the previous section, it is specified
that the ``cmake_find_package`` one is prefered but an issue in the Conan Center
Index makes it unusable for Pybind11 (see `here
<https://github.com/conan-io/conan-center-index/pull/4445>`_ for more details).
For the same reason, the Pybind11 package is not a runtime dependency of the
``Pylene-numpy`` library and should be specified as a dependency of the
extension project. Finally, the `fPIC` option is specified as an option for
Pylene, which enable to have the ``Pylene-numpy`` library during the
dependencies installation.

Then, below is the ``CMakeLists.txt``:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.14)
    project(pylene_extension)

    if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
        message(FATAL_ERROR "Conan dit not run. Please run conan install.")
    endif()
    include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")

    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_BINARY_DIR}" "${CONAN_BUILD_DIRS_PYBIND11}")
    find_package(Pylene REQUIRED)

    include(pybind11Install)
    pybind11_add_module(pylene_extension)
    target_sources(pylene_extension PRIVATE pylene_extension.cpp)
    target_link_libraries(pylene_extension PUBLIC Pylene::Pylene-numpy)


Step 2: Writing an extension
----------------------------

Below is an exemple of an extension module:

.. code-block:: cpp

    #include <pln/core/image_cast.hpp>
    #include <mln/core/image/ndimage.hpp>
    #include <mln/core/range/foreach.hpp>

    #include <pybind11/pybind11.h>

    #include <stdexcept>

    void iota(mln::ndbuffer_image arg_img)
    {
        auto img = arg_img.cast_to<std::uint8_t, 2>();
        if (!img)
            throw std::invalid_argument("iota: input image should be a 2D uint8 image");

        std::uint8_t i = 0;
        mln_foreach(auto p, img->domain())
        {
            (*img)(p) = i;
            i = i == 255 ? 0 : i + 1;
        }
    }

    PYBIND11_MODULE(pylene_extension, m)
    {
        pln::init_pylena_numpy(m);
        m.def("iota", &iota);
    }

There are a few important things to note about this code. At first,
**the header** ``<pln/core/image_cast.hpp>`` 
**must be included in all the compilation unit of the extension module**.
In this header, there is the ``type_caster`` converting Pylene images into Numpy
arrays and inversely. If not included, some undefined behavior may happen.

Then, a ``iota`` function is defined. This function fills inplace a 2D image
with unsigned element on 8 bits. The argument of this function is a
``mln::ndbuffer_image``, the type-erased version of the Pylene images, in which
the type and the dimension of the image are stored dynamically. However, it is
not considered as an image by the library and cannot be manipulated directly: it
has to be casted to a Pylene image, as it is done in the first lines of the
function, thanks to the ``cast_to`` method. This method takes two template
parameters: the type of the image and its dimension. If it matches the ones
stored dynamically, a pointer to the converted image is returned. Else,
``nullptr`` is returned by the method.

Finally, the last lines define the Python module, thanks to Pybind11. It is
important to note that the ``pln::init_pylena_numpy`` is called at the first
line of the module extension.
**It must be called at the first line of the module definition for an extension module.**

Step 3: Using the extension
---------------------------

Finally, below is an example of how to use the simple module developed above.

>>> from pylene_extension import iota
>>> import numpy as np
>>> img = np.zeros((10, 10)).astype(np.float64)
>>> iota(img)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: iota: input image should be a 2D uint8 image
>>> img = img.astype(np.uint8)
>>> iota(img)
>>> img
array([[ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9],
       [10, 11, 12, 13, 14, 15, 16, 17, 18, 19],
       [20, 21, 22, 23, 24, 25, 26, 27, 28, 29],
       [30, 31, 32, 33, 34, 35, 36, 37, 38, 39],
       [40, 41, 42, 43, 44, 45, 46, 47, 48, 49],
       [50, 51, 52, 53, 54, 55, 56, 57, 58, 59],
       [60, 61, 62, 63, 64, 65, 66, 67, 68, 69],
       [70, 71, 72, 73, 74, 75, 76, 77, 78, 79],
       [80, 81, 82, 83, 84, 85, 86, 87, 88, 89],
       [90, 91, 92, 93, 94, 95, 96, 97, 98, 99]], dtype=uint8)