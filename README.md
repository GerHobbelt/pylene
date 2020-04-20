# What is Pylene ?

Pylene is a fork of [Milena](http://www.lrde.epita.fr/olena), an image processing
library targeting genericity and efficiency. Pylene is a modernized version of
Milena with the following objectives:

* Simplicity: both python bindings and simple C++ syntax
* Efficiency: write algorithms in a simple way and run them as if they were written in C. We follow one guideline: zero-cost abstraction.
* Genericity: write algorithms that are able to run on many kind of images with, yet, zero-cost abstraction.
* Interopability: run pylene algorithms on image coming from external libraries (even on your own image type).

# Requirements
Pylene is developed in modern C++. You need a modern C++ compatible compiler:

* GCC 7
* Clang 6
* Microsoft Visual Studio 2017

External dependancies:
[FreeImage](www.freeimage.sourceforge.net) (only for IO)

# Status
* GCC 8.2.0
* Clang 6.0.1
* MSVC 15.8.1

[![pipeline status](https://gitlab.lrde.epita.fr/olena/pylene/badges/dev/pipeline.svg)](https://gitlab.lrde.epita.fr/olena/pylene/commits/dev)



# Documentation

Link to the [C++ Documentation](http://olena.pages.lrde.epita.fr/pylene/)

## Presentations
Other resources (mind the dates, the library probably has changed since then):

* [A Modern C++ Library for Generic and Efficient Image Processing (GTGDMM'18)](https://www.lrde.epita.fr/dload/presentations/2018-06-19-geraud.2018.gtgdmmm.pdf)
