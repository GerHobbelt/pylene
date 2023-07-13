Paste
=====

Include :file:`<mln/core/algorithm/paste.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: void paste(InputImage src, OutputImage dst)

    Paste the values from the source image `src` to destination image `dst` considering the location.
    This is equivalent to the following code::

        for (auto px : src.pixels())
            dst(px.point()) = px.val();

    The source and destination images should not alias. 
    This function has a parallel implementation, see following section for an example.


    :param src: The source image
    :param dst: The destination image
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam OutputImage: A model of :cpp:concept:`OutputImage` and :cpp:concept:`AccessibleImage`.

    .. rubric:: Preconditions

    The domain of `src` must be include in the domain of `dst`.
    

Examples
--------

#. Paste a part of the image into a larger one::

    mln::image2d<mln::rgb8> f = lena();
    mln::image2d<mln::rgb8> g(f, mln::init());
    mln::box2d roi = {{50,50}, {100,100}};
    mln::paste(mln::view::clip(f, roi), g);

#. Using parallel paste to paste a part of the image into a larger one::

    mln::image2d<mln::rgb8> f = lena();
    mln::image2d<mln::rgb8> g(f, mln::init());
    mln::box2d roi = {{50,50}, {100,100}};
    mln::parallel::paste(mln::view::clip(f, roi), g);
 

Complexity
----------

Linear in the number of pixels in `src`.
 