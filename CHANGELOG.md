# CHANGELOG

## Version 0.2

### Maintainer

* Edwin Carlinet <edwin1.carlinet@epita.fr>
* Baptiste Esteban <baptiste.esteban@epita.fr>
* Michaël Roynard <michael.roynard@epita.fr>

### Contributors

* Philippe Bernet <philippe.bernet@epita.fr>
* Alexandre Bourquelot <alexandre.bourquelot@epita.fr>
* Quentin Kaci <quentin.kaci@epita.fr>
* Victor Simonin <vsimonin@lrde.epita.fr>
* Célian Gossec <cgossec@lrde.epita.fr>

### Features

* Parallel implementation for pointwise algorithms
* SIMD erosion/dilation/transposition
* Partitionning hierarchy :
  * Alphatree
  * Hierachical watersheds
  * Silency maps
  * Horizontal cut
* FITS Image reader
* Multivariate Tree of Shapes
* Scribo module
  * line detection on document images
* Watershed and waterfall based on user input markers
* Hough line transform
* Implement the subtractive rule for component-tree filtering
* Conan v2 compatibility

### Various fixes

* Adding padding facilities into the library
* New transform function signatures taking advantage of concepts
* Fix bug  #72
* Pylene <-> NumPy image conversion
* Update Python module documentation
* Tile type for buffer primitive
* Improve alphatree in the library
* New conan/cmake targets
* Make some fixes in the hierarchical representations of Pylene
* Add compilers in the CI + split Pylene-core into components
* Fix canonization of the alphatree and watershed hierarchy
* Fix the input type checking of the numpy array type caster
* Fix missing traits indexer for signed MM operators.
* Fix Conan deployment after the addition of the Scribo component
* Fix issue #90
* Write documentation about the IO operations
* Remove dependencies on boost::concepts
* Check minimum compiler version for Clang and GCC
* Fix bug in cvxhull impl
* Weighted hough transform
* Update Windows CI and package FreeImage
* Fixing deployment induced by FreeImage packaging
* New version of the segment detector in Scribo module.
* Update range-v3 to version 0.12.0
* Fix horizontal cut issue
* Fix Conan recipes due to update to 1.56.0
* Update according to Conan v2
* Fix conan create on GCC 10
* feat(scribo): update
* Fix CI to publish into public/${branch_name} for all branches.
