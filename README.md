# What is Pylene ?


[![pipeline status](https://gitlab.lrde.epita.fr/olena/pylene/badges/master/pipeline.svg)](https://gitlab.lrde.epita.fr/olena/pylene/-/commits/master)
[![coverage report](https://gitlab.lrde.epita.fr/olena/pylene/badges/master/coverage.svg)](https://gitlab.lrde.epita.fr/olena/pylene/-/commits/master)


Pylene is a fork of [Milena](http://www.lrde.epita.fr/olena), an image processing
library targeting genericity and efficiency. It provided mostly Mathematical Morphology building blocs for image
processing pipelines. The library has the following objectives in mind:

* Simplicity: both python bindings and simple C++ syntax
* Efficiency: write algorithms in a simple way and run them as if they were written in C. We follow one guideline: zero-cost abstraction.
* Genericity: write algorithms that are able to run on many kind of images with, yet, zero-cost abstraction.
* Interopability: run pylene algorithms on image coming from external libraries (even on your own image type).


# License

Under [Mozilla Public License, v. 2.0](http://mozilla.org/MPL/2.0/).

# Prerequisited
Pylene is developed in modern C++ so you need a recent C++ compiler. The followings compilers are currently supported
and tested:

* GCC 10, 11, 12
* Clang 12, 13, 14, 15
* ~~Microsoft Visual Studio 2019~~ (Dependencies issue from Conan Center Index)

This project relies on some third part libraries, which are handled by the [Conan Package Manager](https://conan.io/) (version 2.0.1 minimum)

# Quick Start

## Installation

The preferred and strongly recommended way to use Pylene is using Conan, a C++ package manager, and CMake. For using the
library by other means, refer to the [documentation](http://olena.pages.lre.epita.fr/pylene/next/tutorial/installation)

1. Add to the Conan remote the artifactory of the LRE, where the Pylene artifact is stored:

```
conan remote add lrde-public https://artifactory.lrde.epita.fr/artifactory/api/conan/lrde-public
```

2. At the root of your project, create a Conan recipe (``conanfile.txt``) containing at least the following:

```
[generators]
CMakeDeps
CMakeToolchain

[requires]
pylene/head@lrde/stable
```

3. Create a ``CMakeLists.txt`` containing at least the following to link Pylene to your executable:

```
target_link_libraries(your_target PUBLIC Pylene::core) # Minimal
```

or to execute the following code sample

```
target_link_libraries(your_target PUBLIC Pylene::io-freeimage)
```

4. Configure the project and build it:

```
$ mkdir build && cd build
$ conan install .. -s build_type=Release -s compiler.cppstd=20 -s compiler.libcxx=libstdc++11 --build missing --build openjpeg*
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=./conan_toolchain.cmake
$ cmake --build .
```

For more information or details about the installation process, please refer to the documentation.

## Code sample


To use the algorithms of the library, just include the corresponding headers. Here is an example to perform a grayscale dilation
with a disc of radius 5.

```cpp
#include <mln/core/image/ndimage.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/core/se/disc.hpp>

#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>


int main(int argc, char** argv)
{
  mln::image2d<uint8_t> input;

  mln::io::imread(argv[1], input);
  auto output = mln::morpho::dilation(input, mln::se::disc(5));
  mln::io::imsave(output, argv[2]);
}
```

## What is in there ?

* Image in any dimension (2D, 3D for medical imaging...)
* Image with any value type (RGB-8, 16-bits, hyperspectral images...)

Morphological algorithms:

* se-based filter (dilation, erosion, openings, top-hats, rank-filters...)
* connected filters (area connected openings/closings, attributes filters; grain filters)
* morphological representations (min/max-tree, Tree of Shapes)
* segmentation algorithms (watersheds)


If you want to see more of what is in the library, have a look on the [C++ Documentation](http://olena.pages.lrde.epita.fr/pylene/)


## Why is Pylene singular when compared to other libraries ?

* It is generic: it allows to write a single algorithm that could be apply on all compatibles image types
* It enables lazy-computing with a view system similar to the Boost Generic Image Library

Have a look on the [introduction](http://olena.pages.lrde.epita.fr/pylene/intro)


# Other ressources

Other resources (mind the dates, the library probably has changed since then):

* [A Modern C++ Point of View of Programming in Image Processing (GPCE'22)](https://www.lrde.epita.fr/dload/papers/roynard.22.gpce.pdf)
* [A Modern C++ Library for Generic and Efficient Image Processing (GTGDMM'18)](https://www.lrde.epita.fr/dload/presentations/2018-06-19-geraud.2018.gtgdmmm.pdf)
