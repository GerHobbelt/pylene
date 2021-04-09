Introduction
============

The Pylene library makes possible to expose its features in Python. To this aim, it provides two components :

* The ``Pylene-numpy`` library, which converts Pylene images into Numpy arrays.
* The ``pylena`` module, which binds some Pylene algorithms in Python.

Currently, the ``pylena`` module is under development and is empty. However, it is possible to create
a Python module using `Pybind11 <https://pybind11.readthedocs.io>`_ and the ``Pylene-numpy`` library. This is described in the next section.

Installation
^^^^^^^^^^^^

Before continuing, it is advised to read :ref:doc:`this section</tutorial/installation>` about the Pylene installation.

This subsection describe how to get the ``Pylene-numpy`` library. Two methods are explained :

* Getting ``Pylene-numpy`` using Conan to use it in a project.
* Building the ``Pylene-numpy`` library from sources.

In any cases, on Linux, make sure to get the Pylene library using the ``Position Independent Code`` compiler option
since Python modules on Linux are dynamic libraries. On Windows, the standard installation steps will install the Python
components.

Using Conan (preferred)
-----------------------

When writing a `conanfile` to get the `Pylene` library, some ``options`` have to be added to get the Python components.
Bellow is an exemple of a `conanfile.txt`, where the `fPIC` option has been added for Pylene.

.. code-block:: text

    [generators]
    cmake_find_package

    [requires]
    pylene/head@lrde/unstable

    [options]
    pylene:fPIC=True # Or pylene:shared=True

Building Pylene using CMake
---------------------------

People willing to build the Pylene library, such as Pylene's developpers, should execute the following command
to build the Python components.

.. code-block:: console

    $ mkdir build && cd build
    $ conan install .. -o pylene:fPIC=True
    $ cmake .. -DPYLENE_BUILD_PYTHON=ON -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    $ make -j4

As explained above, the `fPIC` option has been added to the Conan install command. Futhermore, two CMake's options
are added:

* ``-DPYLENE_BUILD_PYTHON=ON`` to enable the build process of the Python components.
* ``-DCMAKE_POSITION_INDEPENDENT_CODE=ON`` to compile using the `fPIC` flags of the compiler.

If CMake is runned with ``-DPYLENE_BUILD_PYTHON=ON`` but not with ``-DCMAKE_POSITION_INDEPENDENT_CODE=ON``, a warning will be shown during the build system generation
but it will not prevent the build process to run. So during the build process, a linker error should be raised for the building of the ``pylena`` module.