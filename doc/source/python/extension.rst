Writing an extension module
===========================

Step 1: Setup the project
--------------------------

.. code-block:: text

    [generators]
    cmake_find_package
    cmake

    [requires]
    pylene/head@lrde/unstable
    pybind11/2.6.2

    [options]
    pylene:fPIC=True

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.14)
    project(pylene_extension)

    if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
      include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
      conan_set_rpath()
    endif()

    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_BINARY_DIR}" "${CONAN_BUILD_DIRS_PYBIND11}")
    find_package(Pylene REQUIRED)

    include(pybind11Install)
    pybind11_add_module(pylene_extension)
    target_sources(pylene_extension PRIVATE pylene_extension.cpp)
    target_link_libraries(pylene_extension PUBLIC Pylene::Pylene-numpy)

Step 2: Writing an extension
----------------------------

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

Step 3: Using the extension
---------------------------

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