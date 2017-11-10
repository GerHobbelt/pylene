# What is Pylene ?

Pylene is a fork of [Milena](www.lrde.epita.fr/olena), an image processing
library targeting genericity and efficiency. Pylene is a modernized version of
Milena with the following objectives:

* Simplicity: both python bindings and simple C++ syntax
* Efficiency: write algorithms in a simple way and run them as if they were written in C. We follow one guideline: zero-cost abstraction.
* Genericity: write algorithms that are able to run on many kind of images with, yet, zero-cost abstraction.
* Interopability: run pylene algorithms on image coming from external libraries (even on your own image type).


# Requirements
Pylene is developed in modern C++. You need a modern C++ compatible compiler:

* GCC 7
* Clang 4
* Microsoft Visual Studio 2017

External dependancies:
[FreeImage](www.freeimage.sourceforge.net) (only for IO)

# Status
* GCC 7.2
<a href="http://teamcity.lrde.epita.fr/viewType.html?buildTypeId=Olena_PyleneArchLinuxReleaseGcc&guest=1">
<img src="http://teamcity.lrde.epita.fr/app/rest/builds/buildType:(id:Olena_PyleneArchLinuxReleaseGcc)/statusIcon"/>
</a>
* Clang 5.0
<a href="http://teamcity.lrde.epita.fr/viewType.html?buildTypeId=Olena_PyleneArchLinuxReleaseClang&guest=1">
<img src="http://teamcity.lrde.epita.fr/app/rest/builds/buildType:(id:Olena_PyleneArchLinuxReleaseClang)/statusIcon"/>
</a>

# Documentation

Link to the [C++ Documentation](http://buildfarm.lrde.epita.fr/repository/download/Olena_PyleneDocumentation/.lastSuccessful/documentation.zip!/index.html?guest=1)

