Operators (View)
################

Include :file:`<mln/core/image/view/operators.hpp>`

.. cpp:namespace:: mln::view::ops


Arithmetical
============


1. .. cpp:function:: auto operator+(Image ima1, Image ima2)
2. .. cpp:function:: auto operator+(Image ima1, Scalar s)
3. .. cpp:function:: auto operator+(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) + ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) + s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s + ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 + ima2;
        double s = 5.5;
        auto g1 = ima1 + s;
        auto g1 = s + ima1;


1. .. cpp:function:: auto operator-(Image ima1, Image ima2)
2. .. cpp:function:: auto operator-(Image ima1, Scalar s)
3. .. cpp:function:: auto operator-(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) - ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) - s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s - ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 - ima2;
        double s = 5.5;
        auto g1 = ima1 - s;
        auto g1 = s - ima1;


1. .. cpp:function:: auto operator*(Image ima1, Image ima2)
2. .. cpp:function:: auto operator*(Image ima1, Scalar s)
3. .. cpp:function:: auto operator*(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) * ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) * s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s * ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 * ima2;
        double s = 5.5;
        auto g1 = ima1 * s;
        auto g1 = s * ima1;


1. .. cpp:function:: auto operator/(Image ima1, Image ima2)
2. .. cpp:function:: auto operator/(Image ima1, Scalar s)
3. .. cpp:function:: auto operator/(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) / ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) / s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s / ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. warning:: Mind the division by zero.


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 / ima2;
        double s = 5.5;
        auto g1 = ima1 / s;
        auto g1 = s / ima1;


1. .. cpp:function:: auto operator%(Image ima1, Image ima2)
2. .. cpp:function:: auto operator%(Image ima1, Scalar s)
3. .. cpp:function:: auto operator%(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) % ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) % s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s % ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 % ima2;
        double s = 5.5;
        auto g1 = ima1 % s;
        auto g1 = s % ima1;


Logical
=======


1. .. cpp:function:: auto operator&&(Image ima1, Image ima2)
2. .. cpp:function:: auto operator&&(Image ima1, Scalar s)
3. .. cpp:function:: auto operator&&(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) && ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) && s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s && ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 && ima2;
        double s = 5.5;
        auto g1 = ima1 && s;
        auto g1 = s && ima1;


1. .. cpp:function:: auto operator||(Image ima1, Image ima2)
2. .. cpp:function:: auto operator||(Image ima1, Scalar s)
3. .. cpp:function:: auto operator||(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) || ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) || s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s || ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 || ima2;
        double s = 5.5;
        auto g1 = ima1 || s;
        auto g1 = s || ima1;


1. .. cpp:function:: auto operator!(Image ima)

    1. Makes a view where for each pixel value evals to `out(p) = !ima(p)`


    :param ima: Input range


    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        auto g = !ima;


Comparison
==========


1. .. cpp:function:: auto operator<(Image ima1, Image ima2)
2. .. cpp:function:: auto operator<(Image ima1, Scalar s)
3. .. cpp:function:: auto operator<(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) < ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) < s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s < ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 < ima2;
        double s = 5.5;
        auto g1 = ima1 < s;
        auto g1 = s < ima1;


1. .. cpp:function:: auto operator<=(Image ima1, Image ima2)
2. .. cpp:function:: auto operator<=(Image ima1, Scalar s)
3. .. cpp:function:: auto operator<=(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) <= ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) <= s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s <= ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 <= ima2;
        double s = 5.5;
        auto g1 = ima1 <= s;
        auto g1 = s <= ima1;


1. .. cpp:function:: auto operator==(Image ima1, Image ima2)
2. .. cpp:function:: auto operator==(Image ima1, Scalar s)
3. .. cpp:function:: auto operator==(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) == ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) == s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s == ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 == ima2;
        double s = 5.5;
        auto g1 = ima1 == s;
        auto g1 = s == ima1;


1. .. cpp:function:: auto equalFP(Image ima1, Image ima2, double eps)
2. .. cpp:function:: auto equalFP(Image ima1, Scalar s, double eps)
3. .. cpp:function:: auto equalFP(Scalar s, Image ima1, double eps)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = abs(ima1(p) - ima2(p)) < eps`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = abs(ima1(p) - s) < eps`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = abs(s - ima1(p)) < eps`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value
    :param eps: Epsilon precision


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = equalFP(ima1, ima2, 10e-6);
        double s = 5.5;
        auto g1 = equalFP(ima1, s, 10e-6);
        auto g1 = equalFP(s, ima1, 10e-6);


1. .. cpp:function:: auto operator!=(Image ima1, Image ima2)
2. .. cpp:function:: auto operator!=(Image ima1, Scalar s)
3. .. cpp:function:: auto operator!=(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) != ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) != s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s != ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 != ima2;
        double s = 5.5;
        auto g1 = ima1 != s;
        auto g1 = s != ima1;


#. .. cpp:function:: auto operator>(Image ima1, Image ima2)
#. .. cpp:function:: auto operator>(Image ima1, Scalar s)
#. .. cpp:function:: auto operator>(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) > ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) > s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s > ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 > ima2;
        double s = 5.5;
        auto g1 = ima1 > s;
        auto g1 = s > ima1;


1. .. cpp:function:: auto operator>=(Image ima1, Image ima2)
2. .. cpp:function:: auto operator>=(Image ima1, Scalar s)
3. .. cpp:function:: auto operator>=(Scalar s, Image ima1)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) >= ima2(p)`
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima1(p) >= s`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = s >= ima1(p)`.


    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<mln::rgb8> ima1 = ...;
        mln::image2d<mln::rgb8> ima2 = ...;
        auto g1 = ima1 >= ima2;
        double s = 5.5;
        auto g1 = ima1 >= s;
        auto g1 = s >= ima1;


.. cpp:namespace:: mln::view

Conditional
===========


1. .. cpp:function:: auto ifelse(Image ima, Image ima1, Image ima2)
2. .. cpp:function:: auto ifelse(Image ima, Image ima1, Scalar s_else)
3. .. cpp:function:: auto ifelse(Image ima, Scalar s_if, Image ima2)
4. .. cpp:function:: auto ifelse(Image ima, Scalar s_if, Scalar s_else)

    1. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima(p) ? ima1(p) : ima2(p)`.
    2. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima(p) ? ima1(p) : s_else`.
    3. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima(p) ? s_if : ima2(p)`.
    4. Makes a view where for each pixel value evals to :cpp:expr:`out(p) = ima(p) ? s_if : s_else`.


    :param ima: Input range
    :param ima1: Input range
    :param ima2: Input range
    :param s: Scalar value


    .. code::
    
        mln::image2d<int> ima = ...;
        mln::image2d<int> ima1 = ...;
        mln::image2d<int> ima2 = ...;
        auto g1 = view::ifelse(ima == 0, ima1, ima2);
        auto g2 = view::ifelse(ima < 125, 125, ima);
        auto g3 = view::ifelse(ima > 125, ima, 125);
        auto g4 = view::ifelse(ima < 125, 0, 255);


+----------------+---------------+-------------------+----------------------------------------+---------------------------+-------------------------------------------------------------------------------------+
|                |               | *ima, ima1, ima2* | *+, -, \*, /, %, <, <=, ==, !=, >, >=* | *ifelse(ima, ima1, ima2)* | *ifelse(ima, ima1, s_else) / ifelse(ima, s_if, ima2) / ifelse(ima, s_if, s_else)*   |
+================+===============+===================+========================================+===========================+=====================================================================================+
|                | Forward       | X                 | X                                      | X                         | X                                                                                   |
+                +---------------+-------------------+----------------------------------------+---------------------------+-------------------------------------------------------------------------------------+
| **Category**   | Bidirectional | X                 | X                                      | X                         | X                                                                                   |
+                +---------------+-------------------+----------------------------------------+---------------------------+-------------------------------------------------------------------------------------+
|                | Raw           | X                 |                                        |                           |                                                                                     |
+----------------+---------------+-------------------+----------------------------------------+---------------------------+-------------------------------------------------------------------------------------+
|                | Writable      | X                 |                                        | X                         |                                                                                     |
+                +---------------+-------------------+----------------------------------------+---------------------------+-------------------------------------------------------------------------------------+
| **Properties** | Accessible    | X                 | X                                      | X                         | X                                                                                   |
+                +---------------+-------------------+----------------------------------------+---------------------------+-------------------------------------------------------------------------------------+
|                | Indexable     | X                 | X                                      | X                         | X                                                                                   |
+----------------+---------------+-------------------+----------------------------------------+---------------------------+-------------------------------------------------------------------------------------+


Examples
--------

TODO
