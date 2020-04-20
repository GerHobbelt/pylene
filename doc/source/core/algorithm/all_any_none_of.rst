All_of, any_of, none_of
=======================

- Include :file:`<mln/core/algorithm/all_of.hpp>`
- Include :file:`<mln/core/algorithm/any_of.hpp>`  
- Include :file:`<mln/core/algorithm/none_of.hpp>`

.. cpp:namespace:: mln

#. .. cpp:function:: bool all_of(InputImage ima, UnaryPredicate p)

#. .. cpp:function:: bool all_of(InputImage ima)

#. .. cpp:function:: bool any_of(InputImage ima, UnaryPredicate p)

#. .. cpp:function:: bool any_of(InputImage ima)

#. .. cpp:function:: bool none_of(InputImage ima, UnaryPredicate p)

#. .. cpp:function:: bool none_of(InputImage ima)

    * 1,3,5) Checks that all, any or none of the image values verify the preficate
    * 2,4,6) Checks that all, any or none of the image values evaluate to ``true`` 


    :param ima: The image to test
    :param p: The unary predicate that will be applied on values
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam UnaryPredicate: A model of :cpp:concept:`stl::UnaryPredicate`
    :return: True or false

    

Examples
--------

#. Check that all values are non-negative::

    mln::image2d<uint8_t> f = ...;
    bool res1 = mln::all_of(f, [](uint8_t x) { x > 0; }); // Version 1
    bool res2 = mln::all_of(f > 0);

Complexity
----------

Linear in the number of pixels.
