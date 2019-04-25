Functional maths operators
##########################


Include :file:`<mln/core/functional_ops.hpp>`

.. cpp:namespace:: mln::functional

Component-wise
==============


1. .. cpp:function:: auto abs(Scalar s)
2. .. cpp:function:: auto abs(Vector v)

    1. Returns the absolute value of ``s``: :math:`r = \lvert s \rvert`.
    2. Returns the absolute value of each component of ``v``: :math:`r_i = \lvert v_i \rvert`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = -2;
        mln::signed_rgb8 v = {1, -2, 3};
        auto r_s = abs(s); // 2
        auto r_v = abs(v); // {1, 2, 3}


1. .. cpp:function:: auto pow(Scalar s, Scalar p)
2. .. cpp:function:: auto pow(Vector v, Scalar p)

    1. Returns ``s`` exponent ``p``: :math:`r = s^p`.
    2. Returns each component of ``v`` exponent ``p``: :math:`r_i = v_i^p`.

    :param s: Input scalar
    :param v: Input vector
    :param p: Input exponent

    .. code::

        mln::int8 s = -2;
        mln::signed_rgb8 v = {1, -2, 3};
        auto r_s = pow(s, 3); // -8
        auto r_v = pow(v, 3); // {1, -8, 27}


1. .. cpp:function:: auto sqr(Scalar s)
2. .. cpp:function:: auto sqr(Vector v)

    1. Returns the square value of ``s``: :math:`r = s^2`.
    2. Returns the square value of each component of ``v``: :math:`r_i = v_i^2`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = -2;
        mln::signed_rgb8 v = {1, -2, 3};
        auto r_s = sqr(s); // 4
        auto r_v = sqr(v); // {1, 4, 9}


1. .. cpp:function:: auto cbrt(Scalar s)
2. .. cpp:function:: auto cbrt(Vector v)

    1. Returns the cubic root of ``s``: :math:`r = \sqrt[3]{s}`.
    2. Returns the cubic root of each component of ``v``: :math:`r_i = \sqrt[3]{v_i}`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 729;
        mln::signed_rgb8 v = {-0.125, 8, 729};
        auto r_s = cbrt(s); // 9
        auto r_v = cbrt(v); // {-0.5, 2, 9}


1. .. cpp:function:: auto sqrt(Scalar s)
2. .. cpp:function:: auto sqrt(Vector v)

    1. Returns the square root of ``s``: :math:`r = \sqrt{s}`.
    2. Returns the square root of each component of ``v``: :math:`r_i = \sqrt{v_i}`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 81;
        mln::signed_rgb8 v = {-0.25, 4, 9};
        auto r_s = sqrt(s); // 9
        auto r_v = sqrt(v); // {-0.5, 2, 3}


Reduction
=========


1. .. cpp:function:: auto sum(Scalar s)
2. .. cpp:function:: auto sum(Vector v)

    1. Returns ``s``: :math:`r = s`.
    2. Returns the sum of all components of ``v``: :math:`r = \sum{v_i}`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {1, -2, 9};
        auto r_s = sum(s); // 9
        auto r_v = sum(v); // 8


1. .. cpp:function:: auto prod(Scalar s)
2. .. cpp:function:: auto prod(Vector v)

    1. Returns ``s``: :math:`r = s`.
    2. Returns the product of all components of ``v``: :math:`r = \prod{v_i}`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {1, -2, 9};
        auto r_s = prod(s); // 9
        auto r_v = prod(v); // -18


1. .. cpp:function:: auto min(Scalar s)
2. .. cpp:function:: auto min(Vector v)

    1. Returns ``s``: :math:`r = s`.
    2. Returns the minimum of all components of ``v``: :math:`r = \min(v_1, v_2, \dots, v_n)`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {1, -2, 9};
        auto r_s = min(s); // 9
        auto r_v = min(v); // -2


1. .. cpp:function:: auto max(Scalar s)
2. .. cpp:function:: auto max(Vector v)

    1. Returns ``s``: :math:`r = s`.
    2. Returns the maximum of all components of ``v``: :math:`r = \max(v_1, v_2, \dots, v_n)`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {1, -2, 9};
        auto r_s = max(s); // 9
        auto r_v = max(v); // 9


1. .. cpp:function:: auto dot(Vector v1, Vector v2)

    2. Returns the scalar product of ``v1`` by ``v2``: :math:`r = v_1 \cdot v_2`.

    :param v1: Input vector
    :param v2: Input vector

    .. code::

        mln::signed_rgb8 v1= {9, 1, -2};
        mln::signed_rgb8 v2 = {1, -2, 9};
        auto v = dot(v1, v2); // -11


1. .. cpp:function:: auto cross(Vector v1, Vector v2)

    2. Returns the cross product of ``v1`` by ``v2``: :math:`r = v_1 \wedge v_2`.


    :param v1: Input vector
    :param v2: Input vector

    .. code::

        mln::signed_rgb8 v1= {9, 1, -2};
        mln::signed_rgb8 v2 = {1, -2, 9};
        auto v = cross(v1, v2); // {5, -83, -19}


    .. warning:: The cross product is only defined for vectors of dimension 3 and 7.


Norms
=====


1. .. cpp:function:: auto l0norm(Scalar s)
2. .. cpp:function:: auto l0norm(Vector v)

    1. Returns the absolute value norm of ``s``: :math:`r = \lvert s \rvert`.
    2. Returns the absolute value (min) norm of ``v``: :math:`r = \lVert v \rVert_0 = \min(\lvert v_1 \rvert, \lvert v_2 \rvert, \dots, \lvert v_n \rvert)`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {1, -2, 9};
        auto r_s = l0norm(s); // 9
        auto r_v = l0norm(v); // 1


1. .. cpp:function:: auto l1norm(Scalar s)
2. .. cpp:function:: auto l1norm(Vector v)

    1. Returns the absolute value norm of ``s``: :math:`r = \lvert s \rvert`.
    2. Returns the absolute value (sum) norm of ``v``: :math:`r = \lVert v \rVert_1 = \sum{\lvert v_i \rvert}`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {1, -2, 9};
        auto r_s = l1norm(s); // 9
        auto r_v = l1norm(v); // 12


1. .. cpp:function:: auto l2norm(Scalar s)
2. .. cpp:function:: auto l2norm(Vector v)

    1. Returns the euclidean norm of ``s``: :math:`r = \lvert s \rvert`.
    2. Returns the euclidean norm of ``v``: :math:`r = \lVert v \rVert_2 = \sqrt{\sum{v_i^2}}`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {2, -4, 4};
        auto r_s = l2norm(s); // 9
        auto r_v = l2norm(v); // 6


1. .. cpp:function:: auto l2norm_sqr(Scalar s)
2. .. cpp:function:: auto l2norm_sqr(Vector v)

    1. Returns the squared euclidean norm of ``s``: :math:`r = \lvert s \rvert^2`.
    2. Returns the squared euclidean norm of ``v``: :math:`r = \lVert v \rVert_2^2 = \sum{v_i^2}`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {2, -4, 4};
        auto r_s = l2norm_sqr(s); // 81
        auto r_v = l2norm_sqr(v); // 36


1. .. cpp:function:: auto linfnorm(Scalar s)
2. .. cpp:function:: auto linfnorm(Vector v)

    1. Returns the absolute value norm of ``s``: :math:`r = \lvert s \rvert`.
    2. Returns the absolute value (max) norm of ``v``: :math:`r = \lVert v \rVert_{inf} = \max(\lvert v_1 \rvert, \lvert v_2 \rvert, \dots, \lvert v_n \rvert)`.

    :param s: Input scalar
    :param v: Input vector

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {1, -2, 9};
        auto r_s = linfnorm(s); // 9
        auto r_v = linfnorm(v); // 9


1. .. cpp:function:: template <unsigned p> auto lpnorm<p>(Scalar s)
2. .. cpp:function:: template <unsigned p> auto lpnorm<p>(Vector v)

    1. Returns the p-euclidean norm of ``s``: :math:`r = \lvert s \rvert`.
    2. Returns the p-euclidean norm of ``v``: :math:`r = \lVert v \rVert_p = \sqrt[p]{\sum{\lvert v_i \rvert^p}}`.

    :param s: Input scalar
    :param v: Input vector
    :tparam p: Norm exponent

    .. code::

        mln::int8 s = 9;
        mln::signed_rgb8 v = {2, -4, 9};
        auto r_s = lpnorm<4>(s); // 9
        auto r_v = lpnorm<4>(v); // 9.09186...


Distances
=========


1. .. cpp:function:: auto l0dist(Scalar s1, Scalar s2)
2. .. cpp:function:: auto l0dist(Vector v1, Vector v2)

    1. Returns the distance between ``s1`` and ``s2``: :math:`d = \lvert s_1 - s_2 \rvert`.
    2. Returns the distance normalized (min of absolute values) between ``v1`` and ``v2``: :math:`d = \lVert v_1 - v_2 \rVert_0`.

    :param s1: Input scalar
    :param s2: Input scalar
    :param v1: Input vector
    :param v2: Input vector

    .. code::

        mln::int8 s1 = 9;
        mln::int8 s2 = -2;
        mln::signed_rgb8 v1 = {1, 2, 2};
        mln::signed_rgb8 v2 = {3, 6, 6};
        auto d_s = l0dist(s1, s2); // 11
        auto d_v = l0dist(v1, v2); // 2


1. .. cpp:function:: auto l1dist(Scalar s1, Scalar s2)
2. .. cpp:function:: auto l1dist(Vector v1, Vector v2)

    1. Returns the distance between ``s1`` and ``s2``: :math:`d = \lvert s_1 - s_2 \rvert`.
    2. Returns the distance normalized (sum of absolute values) between ``v1`` and ``v2``: :math:`d = \lVert v_1 - v_2 \rVert_1`.

    :param s1: Input scalar
    :param s2: Input scalar
    :param v1: Input vector
    :param v2: Input vector

    .. code::

        mln::int8 s1 = 9;
        mln::int8 s2 = -2;
        mln::signed_rgb8 v1 = {1, 2, 2};
        mln::signed_rgb8 v2 = {3, 6, 6};
        auto d_s = l1dist(s1, s2); // 11
        auto d_v = l1dist(v1, v2); // 10


1. .. cpp:function:: auto l2dist(Scalar s1, Scalar s2)
2. .. cpp:function:: auto l2dist(Vector v1, Vector v2)

    1. Returns the distance between ``s1`` and ``s2``: :math:`d = \lvert s_1 - s_2 \rvert`.
    2. Returns the distance normalized (euclidean) between ``v1`` and ``v2``: :math:`d = \lVert v_1 - v_2 \rVert_2`.

    :param s1: Input scalar
    :param s2: Input scalar
    :param v1: Input vector
    :param v2: Input vector

    .. code::

        mln::int8 s1 = 9;
        mln::int8 s2 = -2;
        mln::signed_rgb8 v1 = {1, 2, 2};
        mln::signed_rgb8 v2 = {3, 6, 6};
        auto d_s = l2dist(s1, s2); // 11
        auto d_v = l2dist(v1, v2); // 6


1. .. cpp:function:: auto l2dist_sqr(Scalar s1, Scalar s2)
2. .. cpp:function:: auto l2dist_sqr(Vector v1, Vector v2)

    1. Returns the squared distance between ``s1`` and ``s2``: :math:`d = \lvert s_1 - s_2 \rvert^2`.
    2. Returns the squared distance normalized (euclidean) between ``v1`` and ``v2``: :math:`d = \lVert v_1 - v_2 \rVert_2^2`.

    :param s1: Input scalar
    :param s2: Input scalar
    :param v1: Input vector
    :param v2: Input vector

    .. code::

        mln::int8 s1 = 9;
        mln::int8 s2 = -2;
        mln::signed_rgb8 v1 = {1, 2, 2};
        mln::signed_rgb8 v2 = {3, 6, 6};
        auto d_s = l2dist_sqr(s1, s2); // 121
        auto d_v = l2dist_sqr(v1, v2); // 36


1. .. cpp:function:: auto linfdist(Scalar s1, Scalar s2)
2. .. cpp:function:: auto linfdist(Vector v1, Vector v2)

    1. Returns the distance between ``s1`` and ``s2``: :math:`d = \lvert s_1 - s_2 \rvert`.
    2. Returns the distance normalized (max of absolute values) between ``v1`` and ``v2``: :math:`d = \lVert v_1 - v_2 \rVert_{inf}`.

    :param s1: Input scalar
    :param s2: Input scalar
    :param v1: Input vector
    :param v2: Input vector

    .. code::

        mln::int8 s1 = 9;
        mln::int8 s2 = -2;
        mln::signed_rgb8 v1 = {1, 2, 2};
        mln::signed_rgb8 v2 = {3, 6, 6};
        auto d_s = linfdist(s1, s2); // 11
        auto d_v = linfdist(v1, v2); // 4


1. .. cpp:function:: template <unsigned p> auto lpdist<p>(Scalar s1, Scalar s2)
2. .. cpp:function:: template <unsigned p> auto lpdist<p>(Vector v1, Vector v2)

    1. Returns the distance between ``s1`` and ``s2``: :math:`d = \lvert s_1 - s_2 \rvert`.
    2. Returns the distance normalized (p-euclidean) between ``v1`` and ``v2``: :math:`d = \lVert v_1 - v_2 \rVert_p`.

    :param s1: Input scalar
    :param s2: Input scalar
    :param v1: Input vector
    :param v2: Input vector
    :tparam p: Norm Exponent

    .. code::
    
        mln::int8 s1 = 9;
        mln::int8 s2 = -2;
        mln::signed_rgb8 v1 = {1, 2, 2};
        mln::signed_rgb8 v2 = {3, 6, 6};
        auto d_s = lpdist<4>(s1, s2); // 11
        auto d_v = lpdist<4>(v1, v2); // 4.79356
