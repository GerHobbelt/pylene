Iota
====

Include :file:`<mln/core/algorithm/iota.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: template <class V> void iota(OutputImage out, V value)

    Fills the image `out` with sequentially increasing values, starting with value and repetitively evaluating ``value++``.
    This is equivalent to the following code::

        for (auto& vout : out.values())
            vout = value++;


    :param out: The output image.
    :param value: Initial value to store, the expression value++ must be well-formed.
    :tparam OutputImage: A model of :cpp:concept:`OutputImage`


Examples
--------

#. Fill an image with increasing gray levels::

    mln::image2d<uint8_t> ima(10, 10);
    mln::iota(ima, 0);
  
 
Complexity
----------

Linear in the number of pixels.
 