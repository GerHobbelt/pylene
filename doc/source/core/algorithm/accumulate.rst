Accumulate
==========

Include :file:`<mln/core/algorithm/accumulate.hpp>`

.. cpp:namespace:: mln

#. .. cpp:function:: template <class V, class BinaryFunction>  V accumulate(InputImage in, V init, BinaryFunction op)

#. .. cpp:function:: template <class Accumulator> auto accumulate(InputImage in, Accumulator accu)

#. .. cpp:function:: template <class Accumulator> auto accumulate(InputImage in, Accumulator accu, Extractor ex)

    Computes a *fold left* of the values of the image over an operation.
    1) Uses a pair binary function and an initial value?
    2) Use an accumulator/extractor pair.

    `(1)` is equivalent to the following code::

        for (auto&& v : in.values())
            init = op(init, v);
        return init;

    `(2)` is equivalent to the following code::

        for (auto&& v : in.values())
            accu.take(v);
        return accu.to_result();

    `(3)` is equivalent to the following code::

        for (auto&& v : in.values())
            accu.take(v);
        return ex(accu);

    :param in: The input image.
    :param op: A binary operator
    :param init: The initial value to sum
    :param accu: The accumulator or feature
    :param ex: The value extractor from the accumulator
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam Accumulator: A model of :cpp:concept:`AccumulatorLike`


Examples
--------

#. Sum up the values of an image::

    mln::image2d<uint8_t> ima = { {1, 2, 3}, {4, 5, 6} };
    int s = mln::accumulate(ima, 0, std::plus<int>());

#. Sum up the values of an image with an accumulator::

    #include <mln/accu/accumulators/sum.hpp>

    mln::image2d<uint8_t> ima = { {1, 2, 3}, {4, 5, 6} };
    int s = mln::accumulate(ima, mln::accu::features::sum<>());

Complexity
----------

Linear in the number of pixels.
 
