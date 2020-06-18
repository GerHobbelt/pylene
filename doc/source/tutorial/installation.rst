Installation
############

.. contents::
   :local:


Pylene can be installed in many Operating Systems. It is currently tested on Windows with
MSVC and Linux with GCC and Clang. Pylene has several dependencies on C++ libraries, in
particular:

* `Intel TBB <https://software.intel.com/en-us/tbb>`_
* `Boost <https://www.boost.org/>`_
* `Range-v3 <https://github.com/ericniebler/range-v3>`_

There are three ways to install the C++ Pylene:

* The preferred and strongly recommended way to use Pylene is using Conan, a C++ package manager that would handle the dependencies.
* The other way is using installing the libraries from sources.


Install (for users)
===================

Install from sources
********************

To build from sources, you will need cmake and install all the dependencies.
Conan can be used to install the dependencies.

Download the latest release from FIXME.

1. Untar the archive::

    tar xf pylene-*-.tar.bz2

#. Install the dependencies::

    mkdir build && cd build
    conan install ..

#. Build the library::

    cmake .. -DCMAKE_BUILD_TYPE=release
    cmake --build . --target Pylene

#. Install the library (as root)::

    cmake --build . --target install


Integration with CMake and Conan (preferred)
********************************************

`Conan <https://docs.conan.io/>`_ is a C++ package manager that can be installed with pip.
The Pylene library is available on our repository https://artifactory.lrde.epita.fr/

1. Add the repository in your conan remotes:

    .. code-block:: shell
    
        conan remote add lrde-public https://artifactory.lrde.epita.fr/artifactory/api/conan/lrde-public


2. In a :file:`conanfile.txt`, add the reference to Pylene::

    pylene/head@lrde/stable

Then, see conan's documentation for how to use the package with your favorite build system. The following steps apply to CMake.

3. Use conan with the **cmake_find_package** generator.

4. In your :file:`CMakeLists.txt`, use **find_package** as usual::

        find_package(Pylene)


The following targets are then available:

* ``Pylene::Pylene``: target to link with when using Pylene. Ex::

    target_link_libraries(MyTarget PRIVATE Pylene::Pylene)


Integration with CMake as a subdirectory
****************************************

You can add the :file:`pylene` library directory into your project and include it in your
:file:`CMakeLists.txt` file::

    add_subdirectory(pylene)

or to exclude it from ``make``, ``make all``, or ``cmake --build ..``::

    add_subdirectory(pylene EXCLUDE_FROM_ALL)

The following targets are then available:

* ``Pylene::Pylene``: target to link with when using Pylene. Ex::

    target_link_libraries(MyTarget PRIVATE Pylene::Pylene)


Integration with CMake from an existing installation
****************************************************

The library installs CMake config files and provide CMake targets.
Use ``find_package`` in your CMakeLists::

    find_package(Pylene CONFIG)

It makes the following targets available:

* ``Pylene::Pylene``: target to link with when using Pylene. Ex::

    target_link_libraries(MyTarget PRIVATE Pylene::Pylene)



Install (for developers)
========================

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

    In the source directory, use:

    .. code-block:: shell

        mkdir build && cd build
        conan install ..
        cmake ..
        cmake --build .














