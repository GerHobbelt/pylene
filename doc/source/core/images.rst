Images
######


Images defined by a vector of point -> value are not accessible (ima(p))

Forsee to add pdim (point) and vdim (value) for dimension constant in iamge concept

.. toctree::
   :hidden:

   values
   points
   pixels
   domains
   indexes
   view/transform


.. contents::
   :local:


Description & Design Overview
=============================



An image `f` is basically a function associating a *point* `p` to a *value* `f(p)`. The pair *(point,value)* is called a
*pixel*. An image is defined over a :cpp:concept:`domain` which is an *ordered range* of points. Depending on the image
properties, an image type enables a set of operations.

Images have pointer semantics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

FIXME
Explain that:

* Images have shallow copy (pointers semantics)
* Owner vs non-owner images

Images (generally) do not have *const-semantics*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

FIXME:
Explain how to pass images to function.


Images are iterables
^^^^^^^^^^^^^^^^^^^^

Most images provide 3 ways of traversing.

* Iterating on *points* through the *domain*
* Iterating on *values*
* Iterating on *pixels*

As a consequence, such an image ``f`` provide:

* ``f.domain()`` The range of the image *points*
* ``f.values()`` The range of the image *values*
* ``f.pixels()`` The range of the image *pixels*

An image can be a :cpp:concept:`BidirectionalImage` and
allows to be traversed in the reverse order. In that case, the
*ranges* are :cpp:concept:`ReversibleRange`.

Below is an example of image traversal::

    mln::image2d<uint8_t> f = {{1,2,3}, {4,5,6}}; 

    mln_foreach(auto p, f.domain())
        std::cout << p << ":" << f(p) << "\n";

    
    mln_foreach(auto v, f.values())
        std::cout << v << "\n";


    mln_foreach(auto px, f.pixels())
        std::cout << px.point() << ":" << px.val() << "\n"; 


    mln_foreach(auto p, mln::ranges::view::reverse(f.domain()))
        std::cout << p << ":" << f(p) << "\n";

    
    mln_foreach(auto v, mln::ranges::view::reverse(f.values()))
        std::cout << v << "\n";


    mln_foreach(auto px, mln::ranges::view::reverse(f.pixels()))
        std::cout << px.point() << ":" << px.val() << "\n"; 

It produces the following output.

.. code-block:: none

   == Traversing forward ==
   Traversing through points.
   [0,0]:1
   [0,1]:2
   [0,2]:3
   [1,0]:4
   [1,1]:5
   [1,2]:6
   Traversing on values.
   1
   2
   3
   4
   5
   6
   Traversing with pixels.
   [0,0]:1
   [0,1]:2
   [0,2]:3
   [1,0]:4
   [1,1]:5
   [1,2]:6

   == Traversing backward ==
   Traversing through points.
   [1,2]:6
   [1,1]:5
   [1,0]:4
   [0,2]:3
   [0,1]:2
   [0,0]:1
   Traversing on values.
   6
   5
   4
   3
   2
   1
   Traversing with pixels.
   [1,2]:6
   [1,1]:5
   [1,0]:4
   [0,2]:3
   [0,1]:2
   [0,0]:1


.. note:: Not all image provide the access of the value associated to a point (the `f(p)` syntax). This is available for :cpp:concept:`AccessibleImage` images. 

.. note::  The macro :c:macro:`mln_foreach` is just a shortcut to iterate over rows in an efficient way.


To iterate over several images in the same time, use :cpp:func:`mln::ranges::view::zip` to pack the value or pixel ranges of each image::


    auto&& v_rng = mln::ranges::view::zip(f1.values(), f2.values())
    mln_foreach((auto [v1, v2]), v_rng)
        std::cout << v1 << " " << v2 << "\n";


Images can be initialized from other images
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is quite common to use temporary images for computation or to return new images whose type depend on the image input
type. As a consequence, we may need to initialize images from other images, this is called *concretization*.

The effects of the *concretization* are:

    Given a image `f`, the concretization of `f` gives an image `g`, with a domain equal to `f`'s, which can store
    values and such that any write to `f` or `g` does not have any side-affect on the other.


Two methods enable to create a new image `g` from a prototype `f`. The second allows to use another value type::

    I f = ...;
    mln::concrete_t<I>  g1 = f.concretize();
    mln::ch_value_t<I, OtherType> g2 = f.template ch_value<OtherType>()

Because the syntax of a call to a template method is quite cumbersome, free functions can be used::

    I f = ...;
    mln::concrete_t<I>            g1 = mln::concretize(f);
    mln::ch_value_t<I, OtherType> g2 = mln::ch_value<OtherType>(f);

.. warning:: The type returned by `concretize` et al. are not images but *image initializers* that support advanced
    parameterization of the initialization.  So you should not use ``auto`` type deduction for variables.


.. topic:: Advanced initialization

*image initializers* follow the `Named parameter Idiom
<https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Named_Parameter>`_ and provides additional init parameters. Note that it may fail to fulfill the requirements, so a status code may be queried to check if everything succeeded.



+------------------------+---------------------------------------------------------------------------------------------+
| ``init(v)``            | Requires the image to be set to `v`                                                         |
+------------------------+---------------------------------------------------------------------------------------------+
| ``adjust(nbh)``        | Requires the extension of the image to be wide enough to hold the :ref:`neighborhood` `nbh` |
+------------------------+---------------------------------------------------------------------------------------------+
| ``get_status(status)`` | Get the status code                                                                         |
+------------------------+---------------------------------------------------------------------------------------------+

Example::

    I f = ...;
    int st_code = 0;
    mln::concrete_t<I> g1 = mln::concretize(f).adjust(mln::c4).get_status(st_code);
    if (st_code == 0)
        // run with g1
    else
        // g1 may not have the adequate extension


.. topic:: Initialization constructor

Advanced initialization require an initialization constructor that enables an image to be initialized from another image
and deduces the initialization configuration from it.

It has the form:

``I(const I& other, mln::image_initializer_params_t params)``
    Initialize from `other` but overrides init-parameters with those from `params`.










Image Concepts
==============

Image-related Concepts
^^^^^^^^^^^^^^^^^^^^^^

* Values

* Point

* Domain

* Pixel



.. raw:: html

         <h2>Values</h2>


:doc:`core/values` are the fundamentals tools to model values held by images.


.. topic:: Concepts

   * :ref:`Value <concept-values-Value>`
   * :ref:`ComparableValue <concept-values-ComparableValue>`
   * :ref:`OrderedValue <concept-values-OrderedValue>`

.. topic:: Predefined values

   * :doc:`core/values/rgba`

.. topic:: Utilities

   TODO channel utilities
..   * :cpp:func:`is_multichannel`
..   * :cpp:func:`dynamic_channel_get`
..   * :cpp:func:`dynamic_channel_size`
..   * :cpp:func:`static_channel_get`
..   * :cpp:func:`static_channel_size`



.. raw:: html

         <h2>Points</h2>


:doc:`core/points` are the fundamentals tools to locate a value inside an images.


.. topic:: Concepts

   * :ref:`Point <concept-points-Point>`




.. raw:: html

         <h2>Pixels</h2>


:doc:`core/pixels` are the fundamentals tools when browsing an images to access both the point and the value.


.. topic:: Concepts

   * :ref:`Pixel <concept-pixels-Pixel>`




.. raw:: html

         <h2>Domains</h2>


:doc:`core/domains` are the fundamental tools when expressing the valid set of points for an image.


.. topic:: Concepts

   * :ref:`Domain <concept-domains-Domain>`




.. raw:: html

         <h2>Indexes</h2>


:doc:`core/indexes` are the lowest level tools to locate a value inside an images.


.. topic:: Concepts

   * :ref:`Index <concept-indexes-Index>`


Image properties
^^^^^^^^^^^^^^^^

Image concepts are *property*-based. An image concept is group of images types sharing some properties.

Image (traversal) category
    The image category describes how an image can be traversed. It can be **forward**, **bidirectional** (can be
    traversed **backward** and **forward**), or **raw** (buffer encoded image with *contiguous* line buffer)
Image Accessibility
    An image is said **accessible** whenever it allows to access the value associated to a point, i.e. it allows to
    write `f(p)`. While it may seem trivial, not all image are accessible (for example image defined by a vector of pair
    *(p, f(p))*.
Image Indexability
    An image is **indexable** whenever it enables to access values with an index localisator. Usually, accessing through
    an index is faster than accessing by a point.
Image Writability
    An image **writable** can be used as an output image to store the result of a processing. Some image are *read-only* as images that compute values on-the-fly and are not buffer-encoded.  



The figure below illustrates image properties and some of the image concept.

.. figure:: /figures/core/image-concepts.svg
    :width: 100%

    Image concepts and properties.



    















--------


.. cpp:namespace:: mln::concepts

.. _concept-images-Image:

Image
#####


.. _concept-images-IndexableImage:

IndexableImage
##############


.. _concept-images-AccessibleImage:

AccessibleImage
###############


.. _concept-images-ReversibleImage:

ReversibleImage
###############


.. _concept-images-RandomAccessImage:

RandomAccessImage
#################


.. _concept-images-ExtendedImage:

ExtendedImage
#############


.. _concept-images-ChValueImage:

ChValueImage
############


.. _concept-images-RawImage:

RawImage
########


.. _concept-images-WritableImage:

WritableImage
#############


.. _concept-images-ViewImage:

ViewImage
#########


.. _concept-images-ConcreteImage:

ConcreteImage
#############




Predefined images
-----------------

.. toctree::

   images/ndimage




Concepts detail
---------------

.. toctree::
   
   concepts/images
