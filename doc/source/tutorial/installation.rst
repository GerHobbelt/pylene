Installation
============

.. contents::
   :local:


Pylene can be installed in many Operating Systems. It is currently tested on Windows with
MSVC and Linux with GCC and Clang. Pylene has several dependencies on C++ libraries, in
particular:

* `Intel TBB <https://software.intel.com/en-us/tbb>`_
* `Boost <https://www.boost.org/>`_
* `Range-v3 <https://github.com/ericniebler/range-v3>`_
* `FreeImage <https://freeimage.sourceforge.io/>`_
* `Fmt <https://fmt.dev>`_



There are three ways to install the C++ Pylene:

* The preferred and strongly recommended way to use Pylene is using Conan, a C++ package manager that would handle the dependencies.
* The other way is using installing the libraries from sources.

In all cases, you have to install *boost* and *FreeImage* using your system package manager.

.. highlight:: shell

Pre-installation
----------------

Install  *FreeImage* and *Boost* with the system package manager of your distribution.
The packages are generally named `freeimage`, `freeimage-devel`, and `boost-devel`



Install from sources
--------------------

To build from sources, you will need cmake and install all the dependencies.
Conan can be used to install the dependencies.

Download the latest release from `Pylene Gitlab <https://gitlab.lrde.epita.fr/olena/pylene/-/releases>`_

1. Untar the archive::

    tar xf pylene-*-.tar.bz2

#. Install the dependencies::

    mkdir build && cd build
    conan install .. -s compiler.cppstd=20

#. Build the library (with no test)::

    cmake .. -DCMAKE_BUILD_TYPE=release -DPYLENE_BUILD_LIBS_ONLY=On
    cmake --build . --target Pylene (or make)

#. Install the library (as root)::

    cmake --build . --target install (or make install)

.. tip::

   You can install in a specific local as *normal* user in your home directory. To install in $HOME/.local
   use::

     cmake . -DCMAKE_INSTALL_PREFIX=$HOME/.local
     cmake --build . --target install


Integration with CMake and Conan (preferred)
--------------------------------------------

`Conan <https://docs.conan.io/>`_ is a C++ package manager that can be installed with pip.
The Pylene library is available on our repository https://artifactory.lrde.epita.fr/

1. Add the repository in your conan remotes::

     conan remote add lrde-public https://artifactory.lrde.epita.fr/artifactory/api/conan/lrde-public


2. In a :file:`conanfile.txt`, add the reference to Pylene::

     pylene/head@lrde/stable

Then, see conan's documentation for how to use the package with your favorite build system. The following steps apply to CMake.

3. Use conan with the **cmake_find_package** generator.

4. In your :file:`CMakeLists.txt`, use **find_package** as usual::

     set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
     find_package(Pylene)


The following targets are then available:

* ``Pylene::Pylene``: target to link with when using Pylene::

    target_link_libraries(MyTarget PRIVATE Pylene::Pylene)


Integration with CMake as a subdirectory
----------------------------------------

You can add the :file:`pylene` library directory into your project and include it in your
:file:`CMakeLists.txt` file::

    add_subdirectory(pylene)

or to exclude it from ``make``, ``make all``, or ``cmake --build ..``::

    add_subdirectory(pylene EXCLUDE_FROM_ALL)

The following targets are then available:

* ``Pylene::Pylene``: target to link with when using Pylene::

    target_link_libraries(MyTarget PRIVATE Pylene::Pylene)


Integration with CMake from an existing installation
----------------------------------------------------

The library installs CMake config files and provide CMake targets.
Use ``find_package`` in your CMakeLists::

    find_package(Pylene)

It makes the following targets available:

* ``Pylene::Pylene``: target to link with when using Pylene::

    target_link_libraries(MyTarget PRIVATE Pylene::Pylene)



Installation (for developers)
-----------------------------

#. Clone the repository::

    git clone git@gitlab.lrde.epita.fr:olena/pylene.git

#. Install developer dependencies using conan.

   * Google Test
   * Google Benchmark
   * range v3
   * FreeImage (non-managed by conan)
   * Boost
   * Python with Sphinx, Matplotlib, Numpy (for the documentation)
   * Conan

#. Use *conan* to install the dependencies not provided by your system.
   This is advised to use the followings options::


        benchmark:shared                = True
        gtest:shared                    = True

   In the source directory, use::

     mkdir build && cd build
     conan install ..
     cmake ..
     cmake --build .


Sample project structure using Pylene
-------------------------------------

See `<https://gitlab.lrde.epita.fr/olena/pylene/-/blob/master/test_package/>`_.


* :file:`conanfile.txt`::

    [requires]
    pylene/head@lrde/stable


* :file:`CMakeLists.txt`::


    project(PyleneTest)

    set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})

    find_package(Pylene REQUIRED)
    add_executable(main main.cpp)
    target_link_libraries(main Pylene::Pylene)


* Build intructions::

    mkdir build && cd build
    conan install .. -g cmake_find_package -s compiler.cppstd=c++20
    cmake ..









