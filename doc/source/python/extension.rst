Extend the ``pylena`` module
============================

When writing algorithms using the Pylene C++ library, it is possible to expand them in Python. Bellow is a detailed example of how to do it.

1. First, write a conanfile as bellow (it is minimal). It is important to set the pylene ``fPIC`` and ``python`` option to True. Else, the ``pylena_numpy`` library will not be available.

.. code-block:: text

    [generators]
    cmake_find_package

    [requires]
    pylene/head@lrde/stable

    [options]
    pylene:fPIC=True
    pylene:python=True

2. Then, write a CMakeLists.txt. To use the pylena_numpy library, you should use the target ``Pylene::pylena_numpy``.

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.8.2)
    project(pylena_ext)

    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR}")

    find_package(Pylene REQUIRED)
    find_package(pybind11 REQUIRED) # Using system pybind11 or conan one (in this case add it in the conanfile.txt)

    pybind11_add_module(pylena_ext module.cpp)
    target_link_libraries(pylena-ext PRIVATE Pylene::Pylene Pylene::pylena_numpy)

3. Finally, bellow is an example of source code.

.. code-block:: cpp

    #include <pln/core/image_cast.hpp> // Should be included in each Compile Unit

    mln::ndbuffer_image iota(/* arguments */)
    {
        mln::ndbuffer_image result;
        
        /* Process iota */

        return result;
    }

    PYBIND11_MODULE(pylena_ext, m)
    {
        m.doc() = "This is an extension of the pylena module";
        m.def("iota", &iota, "Return an image filled using the iota function");
    }

This will give a python module named ``pylena-ext``.

>>> from pylena_ext import iota
>>> img = iota(...)
>>> type(img)
numpy.ndarray
