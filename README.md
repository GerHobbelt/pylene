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
* Clang 5
* Microsoft Visual Studio 2017

External dependancies:
[FreeImage](www.freeimage.sourceforge.net) (only for IO)

# Status
* GCC 7.2
* Clang 5.0

[![pipeline status](https://gitlab.lrde.epita.fr/olena/pylene/badges/dev/pipeline.svg)](https://gitlab.lrde.epita.fr/olena/pylene/commits/dev)



# Documentation

Link to the [C++ Documentation](http://olena.pages.lrde.epita.fr/pylene/)