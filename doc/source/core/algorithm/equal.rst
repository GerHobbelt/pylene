Equal
=====

Include :file:`<mln/core/algorithm/equal.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: bool equal(LhsImage lhs, RhsImage rhs)

    Compare the pixels of `lhs` and `rhs`.
    The result is true if and only if rhs's values equal lhs's values.

    :param lhs: First image to compare
    :param rhs: Second image to compare
    :tparam LhsImage: A model of :cpp:concept:`InputImage`
    :tparam RhsImage: A model of :cpp:concept:`InputImage`
    :return: True of false

    

Examples
--------

#. Compare two images::

    mln::image2d<mln::rgb8> lhs = ...;
    mln::image2d<mln::rgb8> rhs = ...;
    bool is_equal = mln::equal(lhs, rhs);
 

Complexity
----------

Linear in the number of pixels.
 