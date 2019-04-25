Maths (view)
############

Include :file:`<mln/core/image/view/maths.hpp>`

.. cpp:namespace:: mln::view::maths

Component-wise
==============


#. .. cpp:function:: auto abs(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = abs(ima(p))`.
       Refer to :cpp:func:`functional::abs <mln::functional::abs>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::rgb8> g = mln::view::maths::abs(ima);


#. .. cpp:function:: auto pow(Image ima, Scalar s)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = pow(ima(p), s)`.
       Refer to :cpp:func:`functional::pow <mln::functional::pow>` documentation for details.

    :param ima: Input range
    :param s: Exponent

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::rgb8> g = mln::view::maths::pow(ima, 3);


#. .. cpp:function:: auto sqr(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = sqr(ima(p))`.
       Refer to :cpp:func:`functional::sqr <mln::functional::sqr>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::rgb8> g = mln::view::maths::sqr(ima);


#. .. cpp:function:: auto cbrt(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = cbrt(ima(p))`. 
       Refer to :cpp:func:`functional::cbrt <mln::functional::cbrt>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::rgb8> g = mln::view::maths::cbrt(ima);


#. .. cpp:function:: auto sqrt(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = sqrt(ima(p))`. 
       Refer to :cpp:func:`functional::sqrt <mln::functional::sqrt>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::rgb8> g = mln::view::maths::sqrt(ima);


Reduction
=========


#. .. cpp:function:: auto sum(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = sum(ima(p))`. 
       Refer to :cpp:func:`functional::sum <mln::functional::sum>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::sum(ima);


#. .. cpp:function:: auto prod(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = prod(ima(p))`. 
       Refer to :cpp:func:`functional::prod <mln::functional::prod>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::prod(ima);


#. .. cpp:function:: auto min(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = min(ima(p))`. 
       Refer to :cpp:func:`functional::min <mln::functional::min>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::min(ima);


#. .. cpp:function:: auto max(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = max(ima(p))`. 
       Refer to :cpp:func:`functional::max <mln::functional::max>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::max(ima);


#. .. cpp:function:: auto dot(Image ima1, Image ima2)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = dot(ima1(p), ima2(p))`. 
       Refer to :cpp:func:`functional::dot <mln::functional::dot>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::dot(ima1, ima2);


#. .. cpp:function:: auto cross(Image ima1, Image ima2)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = cross(ima1(p), ima2(p))`. 
       Refer to :cpp:func:`functional::cross <mln::functional::cross>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        mln::image2d<mln::rgb8> g = mln::view::maths::cross(ima1, ima2);


Norms
=====


#. .. cpp:function:: auto l0norm(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = l0norm(ima(p))`. 
       Refer to :cpp:func:`functional::l0norm <mln::functional::l0norm>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::l0norm(ima);


#. .. cpp:function:: auto l1norm(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = l1norm(ima(p))`. 
       Refer to :cpp:func:`functional::l1norm <mln::functional::l1norm>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::l1norm(ima);


#. .. cpp:function:: auto l2norm(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = l2norm(ima(p))`. 
       Refer to :cpp:func:`functional::l2norm <mln::functional::l2norm>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::l2norm(ima);


#. .. cpp:function:: auto l2norm_sqr(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = l2norm_sqr(ima(p))`. 
       Refer to :cpp:func:`functional::l2norm_sqr <mln::functional::l2norm_sqr>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::l2norm_sqr(ima);


#. .. cpp:function:: auto linfnorm(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = linfnorm(ima(p))`. 
       Refer to :cpp:func:`functional::linfnorm <mln::functional::linfnorm>` documentation for details.

    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::linfnorm(ima);


#. .. cpp:function:: template <unsigned V> auto lpnorm<V>(Image ima)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = lpnorm<V>(ima(p))`. 
       Refer to :cpp:func:`functional::lpnorm <template <unsigned p> mln::functional::lpnorm<p>>` documentation for details.

    :param ima: Input range
    :tparam V: Norm exponent

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::lpnorm<4>(ima);




Distances
=========


#. .. cpp:function:: auto l0dist(Image ima1, Image ima2)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = l0dist(ima1, ima2)`. 
       Refer to :cpp:func:`functional::l0dist <mln::functional::l0dist>` documentation for details.

    :param ima1: Input range
    :param ima2: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::l0dist(ima1, ima2);


#. .. cpp:function:: auto l1dist(Image ima1, Image ima2)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = l1dist(ima1, ima2)`. 
       Refer to :cpp:func:`functional::l1dist <mln::functional::l1dist>` documentation for details.

    :param ima1: Input range
    :param ima2: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::l1dist(ima1, ima2);


#. .. cpp:function:: auto l2dist(Image ima1, Image ima2)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = l2dist(ima1, ima2)`. 
       Refer to :cpp:func:`functional::l2dist <mln::functional::l2dist>` documentation for details.

    :param ima1: Input range
    :param ima2: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::l2dist(ima1, ima2);


#. .. cpp:function:: auto l2dist_sqr(Image ima1, Image ima2)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = l2dist_sqr(ima1, ima2)`. 
       Refer to :cpp:func:`functional::l2dist_sqr <mln::functional::l2dist_sqr>` documentation for details.

    :param ima1: Input range
    :param ima2: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::l2dist_sqr(ima1, ima2);



#. .. cpp:function:: auto linfdist(Image ima1, Image ima2)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = linfdist(ima1, ima2)`. 
       Refer to :cpp:func:`functional::linfdist <mln::functional::linfdist>` documentation for details.

    :param ima1: Input range
    :param ima2: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::linfdist(ima1, ima2);


#. .. cpp:function:: template <unsigned V> auto lpdist<V>(Image ima1, Image ima2)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = lpdist<V>(ima1(p), ima2(p))`. 
       Refer to :cpp:func:`functional::lpdist <template <unsigned p> mln::functional::lpdist<p>>` documentation for details.

    :param ima1: Input range
    :param ima2: Input range
    :tparam V: Norm Exponent

    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        mln::image2d<mln::uint8> g = mln::view::maths::lpdist<4>(ima1, ima2);


+----------------+---------------+-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|                |               | *ima, ima1, ima2* | *abs, pow, sqr, cbrt, sqrt, sum, prod, min, max, dot, cross, l0norm, l1norm, l2norm, l2norm_sqr, linfnorm, lpnorm, l0dist, l1dist, l2dist, l2dist_sqr, linfdist, lpdist* |
+================+===============+===================+==========================================================================================================================================================================+
|                | Forward       | X                 | X                                                                                                                                                                        |
+                +---------------+-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| **Category**   | Bidirectional | X                 | X                                                                                                                                                                        |
+                +---------------+-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|                | Raw           | X                 |                                                                                                                                                                          |
+----------------+---------------+-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|                | Writable      | X                 |                                                                                                                                                                          |
+                +---------------+-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| **Properties** | Accessible    | X                 | X                                                                                                                                                                        |
+                +---------------+-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|                | Indexable     | X                 | X                                                                                                                                                                        |
+----------------+---------------+-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+


Examples
--------

TODO
