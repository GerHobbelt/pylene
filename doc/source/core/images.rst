Images
######


Images defined by a vector of point -> value are not accessible (ima(p))

Forsee to add pdim (point) and vdim (value) for dimension constant in iamge concept


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
    mln::concrete_t<I>            g1 = mln::imconcretize(f);
    mln::ch_value_t<I, OtherType> g2 = mln::imchvalue<OtherType>(f);

.. warning:: The type returned by `concretize` et al. are not images but *image builders* that support advanced
    parameterization of the initialization.  So you should not use ``auto`` type deduction for variables (or by calling explicitly ``build()`` - see below)::
 
        I f = ...;
        auto g1 = mln::imconcretize(f).build();
        auto g2 = mln::imchvalue<OtherType>(f).build();   


.. rubric:: Advanced initialization with image builders

*image builders* (see :doc:`image_builder`) follow the `Named parameter Idiom
<https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Named_Parameter>`_ and provides additional init parameters. Note that
it may fail to fulfill the requirements, so a status code may be queried to check if everything succeeded. 



+-------------------------+----------------------------------------------------------------------------------------------------------+
| ``set_init_value(v)``   | Requires the image to be set to `v`                                                                      |
+-------------------------+----------------------------------------------------------------------------------------------------------+
| ``set_border(k)``       | Requires the image to have border of size `k`.                                                           |
+-------------------------+----------------------------------------------------------------------------------------------------------+
| ``adjust(se)``          | Requires the extension of the image to be wide enough to hold the :cpp:concept:`StructuringElement` `se` |
+-------------------------+----------------------------------------------------------------------------------------------------------+
| ``get_status(&status)`` | Get the status code                                                                                      |
+-------------------------+----------------------------------------------------------------------------------------------------------+
| ``build()``             | Build and return the new image.                                                                          |
+-------------------------+----------------------------------------------------------------------------------------------------------+

Example::

    I f = ...;
    mln::image_build_error_code st_code;
    mln::concrete_t<I> g1 = mln::imconcretize(f).adjust(mln::c4).get_status(&st_code);
    if (st_code == 0)
        // run with g1
    else
        // g1 may not have the adequate extension


.. rubric:: Initialization constructor

Advanced initialization require an initialization constructor that enables an image to be initialized from another image
and deduces the initialization configuration from it.

It has the form:

``I(const I& other, mln::image_build_params params)``
    Initializes from `other` but overrides init-parameters with those from `params`.







Image Concepts
==============

Image-related Concepts
^^^^^^^^^^^^^^^^^^^^^^

In the introduction, we have seen that an image *f* is function associating **points** to **values**. **Values** are
simple :cpp:concept:`std::Regular` types. **Points** are also :cpp:concept:`std::Regular` but are also
:cpp:concept:`StrictTotallyOrdered` because they are the basis of *domains*.

.. cpp:namespace:: mln::concepts
.. cpp:concept:: template <typename D> Domain

    A *domain* is a :cpp:concept:`std::Range` of *points* which is totally ordered (this ensures a traversal order of
    pixels). It also supports the following extra-operations.

    Let `dom` be an instance of `D` and `p` a point of type `range_value_type<Domain>`.

    +----------------+------------------------------------------+
    | ``dom.has(p)`` | Return true is `p` is contained in `dom` |
    +----------------+------------------------------------------+


The **pixels** represent a pair *(point, value)*.  They provide an efficient and convenient way to iterate over the
*domain* and image *values* in the same time.


.. cpp:concept:: template <typename P> Pixel

    Represent the pair *(point, value)* of an image. It is a :cpp:concept:`std::SemiRegular` type with
    the following accessors:

    +-----------------+------------------------------------+
    | ``pix.val()``   | Get the pixel value                |
    +-----------------+------------------------------------+
    | ``pix.point()`` | Get the pixel point localisator    |
    +-----------------+------------------------------------+


    It also provides the following methods:

    .. cpp:function:: void shift(point_type dp)

        Move the pixel by a shift `dp`


.. cpp:concept:: template <typename P> IndexablePixel

    Same as :cpp:concept:`Pixel` but enables extra features for pixels of :cpp:concept:`IndexableImage`.

    +-----------------+----------------------------------+
    | ``pix.index()`` | Get the pixel index localisator  |
    +-----------------+----------------------------------+


    .. cpp:function:: void shift(point_type dp, index_type dk)

        Move the pixel by a shift `dp` with pre-computed index `dk`.


.. cpp:concept:: template <typename P> WritablePixel

    Same as :cpp:concept:`Pixel` but supports writing for pixels of :cpp:concept:`OutputImage`.

    **Valid expressions**

    +---------------------------+---------------------+
    | :cpp:expr:`pix.val() = v` | Write the value `v` |
    +---------------------------+---------------------+



Image Properties
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
    An image **writable** can be used as an output image to store the result of a processing. Some image are *read-only*
    as images that compute values on-the-fly and are not buffer-encoded.



The figure below illustrates image properties and some of the image concept.

.. figure:: /figures/core/image-concepts.svg
    :width: 100%

    Image concepts and properties.



Image Concept
^^^^^^^^^^^^^

#. .. cpp:concept:: template <class I>  Image
#. .. cpp:concept:: template <class I>  InputImage
#. .. cpp:concept:: template <class I>  ForwardImage


    **Image** (also **ForwardImage** and **InputImage**) is the minimal concept for modeling images. It provides *read*
    access and multi-pass traversal (not that in the STL *input ranges* are not necessary *forward ranges*). Images are
    :cpp:concept:`std::CopyConstructible` and :cpp:concept:`std::MoveConstructible` so that they can be passed by copy
    in algorithms or copied in image adapters.

    .. rubric:: `Type definition`
        :class: concept-typedefs


    .. table::
        :class: full
        :widths: auto

        +---------------------+----------+------------+---------------------------------------+
        |        Type         |   Abbr   | Definition |             Requirements              |
        +=====================+==========+============+=======================================+
        | `point_type`        | ``P``    |            |                                       |
        +---------------------+----------+------------+---------------------------------------+
        | `domain_type`       |          |            | A model of :cpp:concept:`Domain`      |
        +---------------------+----------+------------+---------------------------------------+
        | `value_type`        | ``V``    |            |                                       |
        +---------------------+----------+------------+---------------------------------------+
        | `reference`         | ``VRef`` |            |                                       |
        +---------------------+----------+------------+---------------------------------------+
        | `pixel_type`        | ``Pix``  |            | A model of :cpp:concept:`Pixel`       |
        +---------------------+----------+------------+---------------------------------------+
        | `concrete_type`     |          |            | A model of :cpp:concept:`OutputImage` |
        +---------------------+----------+------------+---------------------------------------+
        | `ch_value_type<V2>` |          |            | A model of :cpp:concept:`OutputImage` |
        +---------------------+----------+------------+---------------------------------------+


    * The type of the domain must be compatible with the image, that is, the type of points of the domain are convertible
      to image point type
    * The type of pixel must be compatible with the image typedefs, that is, given a pixel `pix` of type `Pix`,
      ``pix.val()`` must be convertible to ``VRef`` and  ``pix.point()`` convertible to ``P``


    .. rubric:: Traits

    .. table::
        :class: full
        :widths: auto

        +----------------------+------------------------+---------------------------------------------+
        |    Type Property     |         Value          |                 Description                 |
        +======================+========================+=============================================+
        | `category`           | **forward_image_tag**  | Tag for the traversal category of the image |
        +----------------------+------------------------+---------------------------------------------+
        | `extension_category` | **none_extension_tag** | Tag for extension category of the image     |
        +----------------------+------------------------+---------------------------------------------+
        | `accessible`         | **std::false_type**    | Tag for the image accessibility property    |
        +----------------------+------------------------+---------------------------------------------+
        | `indexable`          | **std::false_type**    | Tag for the image Indexability property     |
        +----------------------+------------------------+---------------------------------------------+



    .. rubric:: `Valid expression`
        :class: concept-expr

    .. table::
        :class: full
        :widths: auto

        +------------------------+----------------------------------+--------------+-------------------------------------------------------------------------+
        |       Expression       |           Return Type            | Precondition |                                Sementics                                |
        +========================+==================================+==============+=========================================================================+
        | ``ima.domain()``       | ``domain_type``                  |              | The domain of the image.                                                |
        +------------------------+----------------------------------+--------------+-------------------------------------------------------------------------+
        | ``ima.values()``       | :cpp:concept:`std::ForwardRange` |              | A range that allows to traverse image values.                           |
        |                        |                                  |              | The elements of the range are convertible to ``VRef``.                  |
        +------------------------+----------------------------------+--------------+-------------------------------------------------------------------------+
        | ``ima.pixels()``       | :cpp:concept:`std::ForwardRange` |              | A range that allows to traverse image pixels.                           |
        |                        |                                  |              | The elements of the range are convertible to ``Pix``.                   |
        +------------------------+----------------------------------+--------------+-------------------------------------------------------------------------+
        | ``ima.concretize()``   | *auto*                           |              | Returns a concrete image initialized from *this*                        |
        +------------------------+----------------------------------+--------------+-------------------------------------------------------------------------+
        | ``ima.ch_value<V2>()`` | *auto*                           |              | Returns a concrete image initialized from *this* with value type ``V2`` |
        +------------------------+----------------------------------+--------------+-------------------------------------------------------------------------+

Output Image Concept
^^^^^^^^^^^^^^^^^^^^

.. cpp:concept:: template <class I>  OutputImage

    **Output Images** extends the concept :cpp:concept:`InputImage` for images that are **writable**.


    .. rubric:: `Type definition`
        :class: concept-typedefs


    .. table::
        :class: full
        :widths: auto

        +--------------+----------+------------+-----------------------------------------------+
        |     Type     |   Abbr   | Definition |                 Requirements                  |
        +==============+==========+============+===============================================+
        | `reference`  | ``VRef`` |            | An object of type `V` is assignable to `Vref` |
        +--------------+----------+------------+-----------------------------------------------+
        | `pixel_type` | ``Pix``  |            | A model of :cpp:concept:`WritablePixel`       |
        +--------------+----------+------------+-----------------------------------------------+


    .. rubric:: `Valid expression`
        :class: concept-expr

    .. table::
        :class: full
        :widths: auto

        +------------------------+----------------------------------+--------------+-------------------------------------------------------------------------+
        |       Expression       |           Return Type            | Precondition |                                Sementics                                |
        +========================+==================================+==============+=========================================================================+
        | ``ima.values()``       | :cpp:concept:`std::OutputRange`  |              | A range that allows to traverse image values.                           |
        |                        |                                  |              | The elements of the range are convertible to ``VRef``.                  |
        +------------------------+----------------------------------+--------------+-------------------------------------------------------------------------+
        | ``ima.pixels()``       | :cpp:concept:`std::ForwardRange` |              | A range that allows to traverse image pixels.                           |
        |                        |                                  |              | The elements of the range are convertible to ``Pix``.                   |
        +------------------------+----------------------------------+--------------+-------------------------------------------------------------------------+


Bidirectional Image Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. cpp:concept:: template <class I>  BidirectionalImage

    **Bidirectional Images** extends the concept :cpp:concept:`InputImage` for images that are traversed forward and
    backward. Traversing in both direction is required by some algorithms like the chamfer distance transform. Note that
    Bidirectional does not have the same semantic as in the STL. Bidirectional means :cpp:concept:`ReversibleRange`.

    .. rubric:: `Type definition`
        :class: concept-typedefs


    .. table::
        :class: full
        :widths: auto

        +---------------+------+------------+----------------------------------------------------------------------+
        |     Type      | Abbr | Definition |                             Requirements                             |
        +===============+======+============+======================================================================+
        | `domain_type` |      |            | A model of :cpp:concept:`Domain` and  :cpp:concept:`ReversibleRange` |
        +---------------+------+------------+----------------------------------------------------------------------+

    .. rubric:: Traits

    .. table::
        :class: full
        :widths: auto

        +---------------+-----------------------------+---------------------------------------------+
        | Type Property |            Value            |                 Description                 |
        +===============+=============================+=============================================+
        | `category`    | **Bidirectional_image_tag** | Tag for the traversal category of the image |
        +---------------+-----------------------------+---------------------------------------------+


    .. rubric:: `Valid expression`
        :class: concept-expr

    .. table::
        :class: full
        :widths: auto

        +------------------+-------------------------------------+--------------+---------------------------------------------------------+
        |    Expression    |             Return Type             | Precondition |                        Sementics                        |
        +==================+=====================================+==============+=========================================================+
        | ``ima.domain()`` | ``domain_type``                     |              | The domain of the image.                                |
        +------------------+-------------------------------------+--------------+---------------------------------------------------------+
        | ``ima.values()`` | :cpp:concept:`std::ReversibleRange` |              | A range that allows to traverse image values both ways. |
        +------------------+-------------------------------------+--------------+---------------------------------------------------------+
        | ``ima.pixels()`` | :cpp:concept:`std::ReversibleRange` |              | A range that allows to traverse image pixels both ways. |
        +------------------+-------------------------------------+--------------+---------------------------------------------------------+


Accessible Image Concept
^^^^^^^^^^^^^^^^^^^^^^^^

.. cpp:concept:: template <class I>  AccessibleImage

    Images that provides the *accessibility** property. Values can be access from any point.

    .. rubric:: Traits

    .. table::
        :class: full
        :widths: auto

        +----------------------+------------------------+---------------------------------------------+
        |    Type Property     |         Value          |                 Description                 |
        +======================+========================+=============================================+
        | `accessible`         | **std::true_type**     | Tag for the image accessibility property    |
        +----------------------+------------------------+---------------------------------------------+


    .. rubric:: `Valid expression`
        :class: concept-expr

    .. table::
        :class: full
        :widths: auto

        +---------------------+-----------------+-------------------------+--------------------------------------+
        |     Expression      |   Return Type   |      Precondition       |              Sementics               |
        +=====================+=================+=========================+======================================+
        | ``ima(p)``          | `I::reference`  | ``ima.domain().has(p)`` | Value access with bound checking.    |
        +---------------------+-----------------+-------------------------+--------------------------------------+
        | ``ima.pixel(p)``    | `I::pixel_type` | ``ima.domain().has(p)`` | Pixel access with bound checking.    |
        +---------------------+-----------------+-------------------------+--------------------------------------+
        | ``ima.at(p)``       | `I::reference`  |                         | Value access without bound checking. |
        +---------------------+-----------------+-------------------------+--------------------------------------+
        | ``ima.pixel_at(p)`` | `I::pixel_type` |                         | Pixel access without bound checking. |
        +---------------------+-----------------+-------------------------+--------------------------------------+

Indexable Image Concept
^^^^^^^^^^^^^^^^^^^^^^^

.. cpp:concept:: template <class I>  IndexableImage

    **Output Images** extends the concept :cpp:concept:`InputImage` for images that are **writable**.

    Images that provides the *indexability** property. Values can be access through an index. Every point has unique
    index and index may not be compatible between images. In other words,
    (p₁ = p₂) ⇏ (ima.index_of_point(p₁) = ima.index_of_point(p₂). 



    .. rubric:: `Type definition`
        :class: concept-typedefs


    .. table::
        :class: full
        :widths: auto

        +--------------+----------+------------+-----------------------------------------------+
        |     Type     |   Abbr   | Definition |                 Requirements                  |
        +==============+==========+============+===============================================+
        | `index_type` | ``Idx``  |            | A model of :cpp:concept:`std::Integral`       |
        +--------------+----------+------------+-----------------------------------------------+

    .. rubric:: Traits

    .. table::
        :class: full
        :widths: auto

        +----------------------+------------------------+---------------------------------------------+
        |    Type Property     |         Value          |                 Description                 |
        +======================+========================+=============================================+
        | `indexable`          | **std::true_type**     | Tag for the image Indexability property     |
        +----------------------+------------------------+---------------------------------------------+

    .. rubric:: `Valid expression`
       :class: concept-expr

    +------------+-------------+--------------+----------------------------------------------------+
    | Expression | Return Type | Precondition |                     Sementics                      |
    +============+=============+==============+====================================================+
    | ``ima[k]`` | `reference` |              | Returns the value at index *k* (no bound checking) |
    +------------+-------------+--------------+----------------------------------------------------+


    Moreover, if the image is both **Indexable** and **Accessible**, then the following expressions are valid:

    +----------------------------+--------------+--------------+----------------------------------------------------+
    | ``cima.index_of_point(p)`` | `index_type` |              | Get the index of point *p*                         |
    +----------------------------+--------------+--------------+----------------------------------------------------+
    | ``cima.point_at_index(k)`` | `point_type` |              | Get the point at index *p*                         |
    +----------------------------+--------------+--------------+----------------------------------------------------+
    | ``cima.delta_index(dp)``   | `index_type` |              | Get the index difference for a shift of *dp*       |
    +----------------------------+--------------+--------------+----------------------------------------------------+


Image Views
===========

.. toctree::
   view/cast
   view/clip
   view/channel
   view/filter
   view/mask
   view/maths
   view/operators
   view/rgb
   view/transform
   view/zip


Predefined images types
=======================

.. toctree::
   images/ndimage



