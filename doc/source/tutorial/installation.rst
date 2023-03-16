Installation
============

.. caution::

    While the :code:`next` branch is not merged into the :code:`master` branch,
    this installation guide works for the :code:`next` branch **ONLY**, meaning
    it works **ONLY** for the :code:`unstable` conan channel of Pylene.

.. contents::
   :local:

Installation
------------

In this part, we present 3 ways to set up the library. The first two ones aimed
at a user wishing to consume the library in his project whereas the third one is
aimed at a contributor wishing to work in the library itself.

Preliminaries (in all cases)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The library relies on the `Conan <https://conan.io/>`_ package manager and the
`CMake <https://cmake.org>`_ build system generator. In this way, the first
prerequisite is to install Conan. For this, just use the `pip <https://pip.pypa.io>`_
package installer this way

.. code-block:: bash

    $ pip install conan>=2.0.1

Pylene is set up to use the modern version of Conan and does not accept Conan
versions below 2.0.1. When Conan is installed, configure it by creating a
default profile and adding the LRDE artifactory remote:

.. code-block:: bash

    $ conan profile detect
    $ conan remote add lrde-public https://artifactory.lrde.epita.fr/artifactory/api/conan/lrde-public

Pylene artifacts are stored in this ``lrde-public`` remote, as well as some
prebuilt binaries of the dependencies to save some time when fetching the
library.

Install from Conan (Preferred)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. First of all, set up a project by creating a :code:`CMakeLists.txt` and a
   :code:`conanfile.txt`. Below is an example of a :code:`conanfile.txt`:

.. code-block:: text

    [generators]
    CMakeDeps
    CMakeToolchain

    [requires]
    pylene/head@lrde/stable

Some options may be added to the :code:`conanfile.txt` followings the needs of
the consumer project.

2. Then, install the dependencies of the consumer project (in this case, only
   Pylene and its dependencies):

.. code-block::

    $ mkdir build && cd build
    $ conan create .. -s build_type=Release -s compiler.cppstd=20 -s compiler.libcxx=libstdc++11 [--build missing --build openjpeg* [--build freeimage*]]

The :code:`--build` argument are optional and may not be used if the package are
already built. In the previous example, we are in the case where the
dependencies have been download from the `Conan Center <https://conan.io/center/>`_.
However, one can use the prebuilt libraries from the LRDE artifactory by adding
the flag :code:`-r lrde-public`.

If using the Conan Center, the usage of the :code:`openjpeg` library may result
in some linking error, because its binaries have been built on an older libc. In
this case, the :code:`--build openjpeg*` forces the library to be built from
sources instead of using the prebuilt binary. However, if the FreeImage library
has already been built, it may have no effect and you should also build the
FreeImage library from sources.

3. Finally, when the CMake project is ready to configure the consuming project,
execute the following command:

.. code-block:: bash

    $ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=./conan_toolchain.cmake

A sample project using this way to install Pylene is given in the `Sample
Project`_ section of this page.

Install from sources
^^^^^^^^^^^^^^^^^^^^

1. Clone the Pylene repository:

.. code-block:: bash

    $ git clone https://gitlab.lre.epita.fr/olena/pylene

2. In the root of the project, execute the creation of the package:

.. code-block:: bash

    $ conan create . --user lrde --channel stable -s build_type=Release -s compiler.cppstd=20 -s compiler.libcxx=libstdc++11 [--build missing --build openjpeg* [--build freeimage*]]

For more information about the optional :code:`--build` flags, please refer to
the `Install from Conan (Preferred)`_ part of this page.

Library development
^^^^^^^^^^^^^^^^^^^

1. Clone the Pylene repository, create a :code:`build` directory at the root of
   the repository and go into it:

.. code-block:: bash

    $ git clone https://gitlab.lre.epita.fr/olena/pylene
    $ mkdir pylene/build
    $ cd pylene/build

2. Install the dependencies Pylene:

.. code-block:: bash

    $ conan install .. -s build_type=Debug -s compiler.cppstd=20 -s compiler.libcxx=libstdc++11 [--build missing --build openjpeg* [--build freeimage*]]

For more information about the optional :code:`--build` flags, please refer to
the `Install from Conan (Preferred)`_ part of this page.

3. Configure the project with CMake:

.. code-block:: bash

    $ cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./conan_toolchain.cmake

or if at least CMake 3.23 is available:

.. code-block:: bash

    $ cmake .. --preset conan-debug

4. Build the project:

.. code-block:: bash

    $ cmake --build . --target Pylene-core # To build the core library

or one can choose as a target a particular Pylene component.

5. Build the tests:

.. code-block:: bash

    $ cmake --build . --target build-tests # Build the tests
    $ cmake --build . --target test # Execute the test suite

6. Build this documentation:

.. code-block:: bash

    $ cmake --build . --target build-doc

Architecture
------------

Pylene is divided into several components to be modular. These components are
used in Conan and CMake. Below are the components:

* :code:`pylene::core`: contains the core library, without any input/output routines
* :code:`pylene::io-freeimage`: contains the IO routines for classic image formats. These ones are based on the `FreeImage <https://freeimage.sourceforge.io/>`_ library. For more information, look at `IO Module <../io.html>`_
* :code:`pylene::io-fits`: contains the routines to load `FITS <https://fits.gsfc.nasa.gov/fits_primer.html>`_ images. These ones are based on the `cfitsio <https://heasarc.gsfc.nasa.gov/fitsio/>`_ library.
* :code:`pylene::scribo`: contains features related to document analysis and processing.

Sample project
--------------

This sample project produces an executable taking into argument an image
filename whose target image will be opened, eroded by a 2D square of size 10 and
save the output image into a file :code:`out.png`.

Here are the files used in this sample project with their content:

.. code-block:: text
    :caption: :code:`conanfile.txt`

    [generators]
    CMakeDeps
    CMakeToolchain

    [requires]
    pylene/head@lrde/stable

.. code-block:: cmake
    :caption: :code:`CMakeLists.txt`

    cmake_minimum_required(VERSION 3.23)
    project(erosion-cli)

    find_package(pylene REQUIRED)

    add_executable(erosion-cli main.cpp)
    target_link_libraries(erosion-cli PRIVATE Pylene::io-freeimage)

.. code-block:: cpp
    :caption: :code:`main.cpp`

    #include <mln/core/image/ndimage.hpp>
    #include <mln/io/imread.hpp>
    #include <mln/io/imsave.hpp>
    #include <mln/morpho/erosion.hpp>
    #include <mln/core/se/rect2d.hpp>

    #include <iostream>

    int main(int argc, char* argv[])
    {
        if (argc < 2)
        {
            std::cerr << "Usage: " << argv[0] << " input_image\n";
            return 1;
        }

        mln::image2d<std::uint8_t> input;
        mln::io::imread(argv[1], input);
        const auto output = mln::morpho::erosion(input, mln::se::rect2d(10, 10));
        mln::io::imsave(output, "out.png");
        return 1;
    }

To use it, execute the following commands:

.. code-block:: bash

    $ mkdir build && cd build
    $ conan install .. --output-folder=. -s build_type=Release -s compiler.cppstd=20 -s compiler.libcxx=libstdc++11 --build missing --build freeimage* --build openjpeg* # The --build are required when using the conan center openjpeg.
    $ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=./conan_toolchain.cmake
    $ cmake --build .