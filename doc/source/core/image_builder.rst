Image Builder
#############




Image Builder Objects
=====================


*image builders* follow the `Named parameter Idiom <https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Named_Parameter>`_ and provides additional init parameters. Note that
it may fail to fulfill the requirements, so a status code may be queried to check if everything succeeded.


.. doxygenstruct:: mln::image_build_params
    :members:

.. doxygenclass:: mln::image_builder
    :members:

.. doxygenclass:: mln::image_resizer
    :members:

.. doxygenenum:: mln::image_build_error_code 






Free functions
==============

.. doxygenfunction:: mln::imconcretize

.. doxygenfunction:: mln::imchvalue

.. doxygenfunction:: mln::resize



Usage
=====

This example shows how to create a temporary image with sufficient border to perform a stencil pattern (accessing the neighbors of a point).


.. code::

    auto myfunction(I f)
    {
        // Try to adjust the border of the image to hold `nbh`
        // and initialize this image with the value `42`
        mln::image_build_error_code st;
        mln::concrete_t<I> g = imconcretize(f).adjust(nbh).set_init_value(42).get_status(st);
        if (st == mln::IMAGE_BUILD_OK)
        {
            mln_foreach(auto p, g.pixels())
                for (auto nx : nbh(px))
                    // Process safely nx
        }
        else 
        {
            // `g` has not a border large enough to hold the neighborhood.
            // You have to check that the access is valid when accessing the neighbors
            // of a point.  
        }
    }