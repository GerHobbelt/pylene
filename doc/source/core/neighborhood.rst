Neighborhood, Sliding Windows and Structuring Elements
######################################################


Neighborhoods, Structuring Elements (SE), Windows are objects that
enable to iterate locally on the pixels of an image around a given
anchor. There are all the same concept but differ in semantics:

Neighborhoods
  Neighborhoods are usually used to define the connectivity of the
  image, e.g. for 2D images, it is usually the 4-connectivity or the
  8-connectivity.

Structuring Elements
  *Structuring Element* is the common term in Mathematical Morphology to
  define a structure

Windows
  *Sliding window* is the common term to designate a square *Structuring
  Element*. It generally refers to 2D grid.


A structuring element 𝑩 is a range generator: given a point `p` (resp. a pixel `px`) it provides the set of points or
pixels :math:`\mathcal{B}(p)` centered (anchored) in `p`. (Note that `p` does not necessary belong to
:math:`\mathcal{B}(p)`. Two kinds of range may be generated:

* :math:`\mathcal{B}(p)` provides a range of points
* :math:`\mathcal{B}(px)` provides a range of pixels from the same image of `px`


For example, the following code iterates over each point in 4x3 rectangle and their 4-connected neighbors::

  box2d box(3, 4);

  for (auto p : box) {
    std::cout << "The neighbors of " << p << " are: " <<
    for (auto q : c4(p))
       std::cout << q << ", ";
    std::cout << "\n";
  }


.. contents::
   :local:


Concepts
********

.. cpp:namespace:: mln

.. cpp:concept:: template <class N> Neighborhood

                 see :cpp:concept:`StructuringElement`



.. cpp:concept:: template <class SE> StructuringElement


   +-------------------------------+-------------------------------------------------------------------+
   |Categorie                      |Operations                                                         |
   +===============================+===================================================================+
   |Adapatative SE                 |Iterate over SE whose elements are dependant on the current point  |
   |                               |                                                                   |
   +----+--------------------------+-------------------------------------------------------------------+
   |    | Dynamic SE               |Iterate over SE whose elements are constant but known at run time. |
   |    |                          |                                                                   |
   |    |                          |                                                                   |
   |    |                          |                                                                   |
   +----+----+---------------------+-------------------------------------------------------------------+
   |    |    | Static SE           |Iterate over SE whose elements are constant, size known at compile |
   |    |    |                     |time                                                               |
   |    |    |                     |                                                                   |
   |    |    |                     |                                                                   |
   +----+----+----+----------------+-------------------------------------------------------------------+
   |    |    |    | Constant SE    |Iterate over SE whose elements are kwown at compile time.          |
   |    |    |    |                |                                                                   |
   |    |    |    |                |                                                                   |
   +----+----+----+----------------+-------------------------------------------------------------------+

   .. rubric:: Valid Expression



   .. rubric:: Type definition
      :class: concept-typedefs

   +---------------------+--------------------------------+----------------------------------------------+
   |Type                 |           Definition           |Requirements                                  |
   +=====================+================================+==============================================+
   |`SE::category`       |                                |Convertible to `adaptative_neighborhood_tag`  |
   +---------------------+--------------------------------+----------------------------------------------+
   |`SE::is_incremental` |either `std::true_type` or      |                                              |
   |                     |`std:false_type`                |                                              |
   +---------------------+--------------------------------+----------------------------------------------+



2D Structuring Elements
***********************

  .. toctree::
   :maxdepth: 1

   se/disc
   se/rectangle
   se/periodic_lines
