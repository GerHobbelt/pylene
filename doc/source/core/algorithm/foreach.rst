For_each
========

Include :file:`<mln/core/algorithm/for_each.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: void for_each(InputImage in, std::UnaryFunction f)


    Applies the function `f` to every value of `in`. It can be used for in-place transformation.
    For storing the result in another image, see :cpp:func:`mln::transform`.

    This is equivalent to the following code::

        for (auto&& v : in.values())
            f(v);

    This function has a parallel implementation, see following section for an example.

    :param in: The input image.
    :param f: The function to apply on values.
    :tparam InputImage: A model of :cpp:concept:`InputImage`


Examples
--------

#. Add one to each element::

    mln::image2d<uint8_t> ima = { {1, 2, 3}, {4, 5, 6} };
    auto out = mln::for_each(ima, [](uint8_t& x) { x += 1; });
  
#. Using parallel for_each to add one to each element::

    mln::image2d<uint8_t> ima = { {1, 2, 3}, {4, 5, 6} };
    auto out = mln::parallel::for_each(ima, [](uint8_t& x) { x += 1; });
 
Complexity
----------

Linear in the number of pixels.
 